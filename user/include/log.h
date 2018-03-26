#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <string.h>

#define UNUSED __attribute__((unused))

#ifndef NO_COLOR
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"
#else
#define ANSI_COLOR_RED
#define ANSI_COLOR_GREEN
#define ANSI_COLOR_YELLOW
#define ANSI_COLOR_BLUE
#define ANSI_COLOR_MAGENTA
#define ANSI_COLOR_CYAN
#define ANSI_COLOR_RESET
#endif

#define __BASEFILE__                                                           \
  (strrchr(__FILE__, '/')                                                      \
     ? strrchr(__FILE__, '/') + 1                                              \
     : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#if defined(DEBUG) && DEBUG > 0
#define LOG_DEBUG(fmt, args...)                                                \
  fprintf(stdout, ANSI_COLOR_RESET "DEBUG: %s:%d:%s(): " fmt "\n",             \
          __BASEFILE__, __LINE__, __func__, ##args)
#else
#define LOG_DEBUG(fmt, args...) /* Don't do anything in release builds */
#endif

#define LOG_INFO(fmt, args...)                                                 \
  fprintf(stderr,                                                              \
          ANSI_COLOR_GREEN "INFO:  %s:%d:%s(): " fmt "\n" ANSI_COLOR_RESET,    \
          __BASEFILE__, __LINE__, __func__, ##args)

#define LOG_ERROR(fmt, args...)                                                \
  fprintf(stderr,                                                              \
          ANSI_COLOR_RED "ERROR: %s:%d:%s(): " fmt "\n" ANSI_COLOR_RESET,      \
          __BASEFILE__, __LINE__, __func__, ##args)

#endif /* __LOG_H__ */
