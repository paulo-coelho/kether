#include "eth.h"
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

int
eth_init(const char* if_name, uint8_t if_addr[ETH_ALEN], int* if_index)
{
  /* Create the AF_PACKET socket. */
  struct ifreq ifr;
  int          sock = socket(AF_PACKET, SOCK_RAW, htons(PAXOS_TYPE));

  if (sock < 0) {
    perror("socket()");
    return 0;
  }

  /* Get the index number and MAC address of ethernet interface. */
  memset(&ifr, 0, sizeof(ifr));
  strncpy(ifr.ifr_name, if_name, IFNAMSIZ - 1);
  if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
    perror("SIOCGIFINDEX");
    return 0;
  }
  *if_index = ifr.ifr_ifindex;
  if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
    perror("SIOCGIFHWADDR");
    return 0;
  }
  memcpy(if_addr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
  return sock;
}

int
eth_listen(int sock, const char* if_name)
{
  struct ifreq ifr;
  int          s;

  memset(&ifr, 0, sizeof(ifr));
  strncpy(ifr.ifr_name, if_name, IFNAMSIZ - 1);

  /* Set interface to promiscuous mode. */
  if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
    perror("SIOCGIFFLAGS");
    close(sock);
    return 0;
  }
  ifr.ifr_flags |= IFF_PROMISC;
  if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0) {
    perror("SIOCSIFFLAGS");
    close(sock);
    return 0;
  }

  /* Allow the socket to be reused. */
  s = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &s, sizeof(s)) < 0) {
    perror("SO_REUSEADDR");
    close(sock);
    return 0;
  }

  /* Bind to device. */
  if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, if_name, IFNAMSIZ - 1) <
      0) {
    perror("SO_BINDTODEVICE");
    close(sock);
    return 0;
  }

  return 1;
}

int
eth_send(int sock, uint8_t if_addr[ETH_ALEN], int if_index,
         uint8_t dest_addr[ETH_ALEN], const char* msg, size_t len)
{
  char                 buf[ETH_FRAME_LEN] = { 0 };
  size_t               send_len;
  struct ether_header* eh;
  struct sockaddr_ll   sock_addr;

  /* Construct ethernet header. */
  eh = (struct ether_header*)buf;
  memcpy(eh->ether_shost, if_addr, ETH_ALEN);
  memcpy(eh->ether_dhost, dest_addr, ETH_ALEN);
  eh->ether_type = htons(PAXOS_TYPE);
  send_len = sizeof(*eh);

  /* Fill the packet data. */
  if (len + send_len >= ETH_FRAME_LEN)
    len = ETH_FRAME_LEN - send_len;

  memcpy(&buf[send_len], msg, len);
  send_len += len;

  /* Fill the destination address and send it. */
  sock_addr.sll_ifindex = if_index;
  sock_addr.sll_halen = ETH_ALEN;
  memcpy(sock_addr.sll_addr, dest_addr, ETH_ALEN);

  if (sendto(sock, buf, send_len, 0, (struct sockaddr*)&sock_addr,
             sizeof(sock_addr)) < 0) {
    perror("sendto()");
    return 0;
  }

  return 1;
}

size_t
eth_receive(int sock, uint8_t if_addr[ETH_ALEN], uint8_t sndr_addr[ETH_ALEN],
            char* rmsg)
{
  char                 buf[ETH_FRAME_LEN] = { 0 };
  struct ether_header* eh = (struct ether_header*)buf;
  ssize_t              received;

  received = recvfrom(sock, buf, ETH_FRAME_LEN, 0, NULL, NULL);
  if (received <= 0) {
    perror("recvfrom()");
    return 0;
  }
  //  fprintf(stdout,
  //          "%02x:%02x:%02x:%02x:%02x:%02x -> %02x:%02x:%02x:%02x:%02x:%02x, "
  //          "type = %04x ",
  //          eh->ether_shost[0], eh->ether_shost[1], eh->ether_shost[2],
  //          eh->ether_shost[3], eh->ether_shost[4], eh->ether_shost[5],
  //          eh->ether_dhost[0], eh->ether_dhost[1], eh->ether_dhost[2],
  //          eh->ether_dhost[3], eh->ether_dhost[4], eh->ether_dhost[5],
  //          ntohs(eh->ether_type));
  //
  /* Receive only if destination address is my own. */
  if (memcmp(eh->ether_dhost, if_addr, ETH_ALEN) != 0)
    return 0;

  memcpy(sndr_addr, eh->ether_shost, ETH_ALEN);

  received -= sizeof(*eh);
  memcpy(rmsg, buf + sizeof(*eh), received);
  return received;
}
