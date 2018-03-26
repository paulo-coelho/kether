#ifndef __ETH_H__
#define __ETH_H__

#include <stdint.h>
#include <net/ethernet.h>

#define PAXOS_TYPE 0xcafe /* custom type */

int eth_init(const char* if_name, uint8_t if_addr[ETH_ALEN], int* if_index);
int eth_send(int sock, uint8_t if_addr[ETH_ALEN], int if_index, uint8_t dest_addr[ETH_ALEN], const char *msg, size_t len);
size_t eth_receive(int sock, uint8_t if_addr[ETH_ALEN], uint8_t sndr_addr[ETH_ALEN], char *rmsg);
int eth_listen(int sock, const char* if_name);

#endif
