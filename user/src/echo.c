#include "eth.h"
#include "log.h"
#include "stats.h"
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

static void
print_usage(const char* progname)
{
  fprintf(stderr, "usage: %s [-i device] [-d dest-addr] msg\n", progname);
}

int
main(int argc, char** argv)
{
  char*          if_name;
  int            sock;
  int            if_index;
  uint8_t        if_addr[ETH_ALEN];
  uint8_t        dest_addr[ETH_ALEN];
  char          buf[ETH_FRAME_LEN];
  int            opt, i;
  struct timeval before, after;
  long           delta;

  if_name = "enp0s3";

  while ((opt = getopt(argc, argv, "i:d:")) != -1) {
    switch (opt) {
      case 'i':
        if_name = optarg;
        break;
      default: /* '?' */
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
  }

  /* initialize socket */
  if ((sock = eth_init(if_name, if_addr, &if_index)) < 0)
    return EXIT_FAILURE;

  if (eth_listen(sock, if_name)) {
    i = 0;
    gettimeofday(&before, NULL);
    while (1) {
      size_t sz = eth_receive(sock, if_addr, dest_addr, buf);
      if (sz) {
        ++i;
        gettimeofday(&after, NULL);
        if ((delta = delta_tv(&before, &after)) > 5000000) {
          printf("Got %.2f msgs/sec.\n", ((float)i) / (delta * 1e-6));
          before = after;
          i = 0;
        }
        eth_send(sock, if_addr, if_index, dest_addr, buf, sz);
      } else {
        printf("Error while receiving.");
        break;
      }
    }
  }

  close(sock);
  return 0;
}
