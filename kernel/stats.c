#include "stats.h"
#include "log.h"
#include <linux/slab.h>
#include <linux/string.h>

/* structure representing statistical data */
struct stats
{
  int            count;
  long           deltas[MAX_STATS];
  int            counts[MAX_STATS];
  struct timeval init;
};

struct stats*
stats_new()
{
  struct stats* s = kmalloc(sizeof(struct stats), GFP_ATOMIC);
  memset(s, 0, sizeof(struct stats));
  do_gettimeofday(&s->init);
  return s;
}

void
stats_add(struct stats* s, int count)
{
  int            i = s->count;
  struct timeval tv;

  do_gettimeofday(&tv);
  s->deltas[i] = delta_t(&s->init, &tv);
  s->counts[i] = count;
  s->count = (s->count + 1) % MAX_STATS;
}

void
stats_destroy(struct stats* s)
{
  if (s)
    kfree(s);
}

void
stats_save(struct stats* s, const char* file_path, int offset)
{
  int i;

  for (i = 0; i < s->count; i++) {
    LOG_INFO("stats(%d) = %d\t%ld", (i + 1), s->counts[i], s->deltas[i]);
  }
  LOG_INFO("TP = %ld",
           s->counts[s->count - 1] / (s->deltas[s->count - 1] / 1000000));
}

long
delta_t(struct timeval* start, struct timeval* end)
{
  long us;
  us = (end->tv_sec - start->tv_sec) * 1000000;
  if (us < 0)
    return 0;
  us += (end->tv_usec - start->tv_usec);
  return us;
}
