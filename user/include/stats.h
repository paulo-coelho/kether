#ifndef __STATS_H__
#define __STATS_H__

#include <sys/time.h>

#define MAX_STATS 10000

/* structure representing statistical data */
struct stats;

struct stats* stats_new(void);
void          stats_add(struct stats* s, int count);
void          stats_destroy(struct stats* s);
void          stats_save(struct stats* s, const char* file_path, int offset);
long          delta_tv(struct timeval* start, struct timeval* end);

#endif