#include "eth.h"
#include "log.h"
#include <linux/if_ether.h>
#include <linux/net.h>
#include <linux/netdevice.h>
#include <linux/rtnetlink.h>
#include <linux/skbuff.h>
#include <linux/socket.h>
#include <linux/string.h>

struct callback
{
  struct packet_type pt;
  rcv_cb             cb;
};

static struct callback cbs[MAX_PROTO];
static int             cbs_count;

static int packet_recv(struct sk_buff* sk, struct net_device* dev,
                       struct packet_type* pt, struct net_device* dev2);

struct net_device*
eth_init(const char* if_name)
{
  cbs_count = 0;
  memset(cbs, 0, sizeof(cbs));
  return dev_get_by_name(&init_net, if_name);
}

int
eth_listen(struct net_device* dev, uint16_t proto, rcv_cb cb)
{
  int i;

  if (cbs_count == MAX_PROTO) {
    LOG_ERROR("reached maximum numbers of listeners.");
    return 0;
  }
  for (i = 0; i < cbs_count; ++i) {
    if (cbs[i].pt.type == htons(proto))
      break;
  }
  cbs[i].pt.type = htons(proto);
  cbs[i].pt.func = packet_recv;
  cbs[i].pt.dev = dev;
  cbs[i].cb = cb;
  if (i == cbs_count) {
    cbs_count++;
    dev_add_pack(&cbs[i].pt);
  }
  return 1;
}

static int
packet_recv(struct sk_buff* skb, struct net_device* dev, struct packet_type* pt,
            struct net_device* src_dev)
{
  int            i;
  uint16_t       proto = skb->protocol;
  struct ethhdr* eth = eth_hdr(skb);
  char           data[ETH_FRAME_LEN - ETH_HLEN];
  size_t         len = skb->len;

  skb_copy_bits(skb, 0, data, len);

  for (i = 0; i < cbs_count; ++i) {
    if (cbs[i].pt.type == proto) {
      cbs[i].cb(dev, eth->h_source, data, len);
      goto free_skb;
    }
  }
  LOG_ERROR("no callback for protocol number 0x%.4X", proto);
free_skb:
  kfree_skb(skb);
  return 0;
}

int
eth_send(struct net_device* dev, uint8_t dest_addr[ETH_ALEN], uint16_t proto,
         const char* msg, size_t len)
{
  int             ret;
  unsigned char*  data;
  struct sk_buff* skb = alloc_skb(ETH_FRAME_LEN, GFP_ATOMIC);

  skb->dev = dev;
  skb->pkt_type = PACKET_OUTGOING;

  skb_reserve(skb, ETH_HLEN);
  /*changing Mac address */
  struct ethhdr* eth = (struct ethhdr*)skb_push(skb, ETH_HLEN);
  skb->protocol = eth->h_proto = htons(proto);
  memcpy(eth->h_source, dev->dev_addr, ETH_ALEN);
  memcpy(eth->h_dest, dest_addr, ETH_ALEN);

  /* put the data and send the packet */
  if (len > ETH_DATA_LEN)
    len = ETH_DATA_LEN;

  data = skb_put(skb, len);
  memcpy(data, msg, len);
  ret = dev_queue_xmit(skb);
  return !ret;
}

int
eth_destroy(struct net_device* dev)
{
  int i;

  for (i = 0; i < cbs_count; ++i)
    dev_remove_pack(&cbs[i].pt);
  return 1;
}

int
str_to_mac(const char* str, uint8_t daddr[ETH_ALEN])
{
  int values[6], i;
  if (6 == sscanf(str, "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2],
                  &values[3], &values[4], &values[5])) {
    /* convert to uint8_t */
    for (i = 0; i < 6; ++i)
      daddr[i] = (uint8_t)values[i];
    return 1;
  }
  return 0;
}

int
mac_to_str(uint8_t daddr[ETH_ALEN], char* str)
{
  sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x\n", daddr[0], daddr[1], daddr[2],
          daddr[3], daddr[4], daddr[5]);
  return 1;
}
