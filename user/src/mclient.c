#include "eth.h"
#include "log.h"
#include "stats.h"
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

struct my_msg
{
  char msg[32];
  int  id;
};

struct client
{
  int            id;
  struct timeval tv;
};

static char broadcast_addr[ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
static uint8_t       if_addr[ETH_ALEN];
static uint8_t       dest_addr[ETH_ALEN];
static int           if_index;
static int           stop = 0;
static struct stats* st = NULL;
void
save_stats(int sig)
{
  if (sig == SIGINT) {
    LOG_INFO("saving stats and exiting");
    stop = 1;
    if (st) {
      stats_save(st, NULL, 0);
      stats_destroy(st);
    }
    exit(0);
  }
}
static void
print_usage(const char* progname)
{
  fprintf(stderr, "usage: %s [-i device] [-d dest-addr] msg\n", progname);
}

void
run_clients(int sock, int nclients)
{
  int            i, rcv = 0;
  char           buf[ETH_FRAME_LEN];
  struct my_msg  msg = { .msg = "HELLO" }, *tmp;
  struct client* cl = malloc(nclients * sizeof(struct client));

  for (i = 0; i < nclients; ++i) {
    cl[i].id = i;
    gettimeofday(&cl[i].tv, NULL);
    msg.id = i;
    LOG_INFO("client %d sending msgs to %02x:%02x:%02x:%02x:%02x:%02x", i,
             dest_addr[0], dest_addr[1], dest_addr[2], dest_addr[3],
             dest_addr[4], dest_addr[5]);
    eth_send(sock, if_addr, if_index, dest_addr, (char*)&msg, sizeof(msg));
  }

  while (!stop) {
    size_t sz = eth_receive(sock, if_addr, dest_addr, buf);
    if (sz) {
      if (rcv == 0)
        st = stats_new();

      rcv++;
      tmp = (struct my_msg*)buf;
      gettimeofday(&cl[tmp->id].tv, NULL);
      if (rcv % 20000 == 0) {
        stats_add(st, rcv);
        LOG_INFO("got message '%s' with %zu bytes.", tmp->msg, sz);
      }

      for (i = 0; i < nclients; ++i) {
        if (i == tmp->id || delta_tv(&(cl[i].tv), &(cl[tmp->id].tv)) > 500000) {
          msg.id = i;
          eth_send(sock, if_addr, if_index, dest_addr, (char*)&msg,
                   sizeof(msg));
        }
      }
    } else {
      printf("Error while receiving.");
      break;
    }
  }
  free(cl);
  close(sock);
}

int
main(int argc, char** argv)
{
  char* if_name;
  int   sock;
  int   opt, i;
  int   nclients = 1;

  if_name = "enp1s0";
  memcpy(dest_addr, broadcast_addr, ETH_ALEN);

  while ((opt = getopt(argc, argv, "c:i:d:")) != -1) {
    switch (opt) {
      case 'c':
        nclients = atoi(optarg);
        break;
      case 'i':
        if_name = optarg;
        break;
      case 'd': {
        int mac[ETH_ALEN];

        if (ETH_ALEN != sscanf(optarg, "%02x:%02x:%02x:%02x:%02x:%02x", &mac[0],
                               &mac[1], &mac[2], &mac[3], &mac[4], &mac[5])) {
          print_usage(argv[0]);
          return EXIT_FAILURE;
        }
        for (i = 0; i < ETH_ALEN; i++)
          dest_addr[i] = mac[i];
      } break;
      default: /* '?' */
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
  }

  /* initialize socket */
  if ((sock = eth_init(if_name, if_addr, &if_index)) < 0)
    return EXIT_FAILURE;

  signal(SIGINT, save_stats);
  run_clients(sock, nclients);
  return 0;
}
