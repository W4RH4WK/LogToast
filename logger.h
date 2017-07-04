#ifndef LOGGER_H_
#define LOGGER_H_

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*                                                  _.---._
 *                                              _.-~       ~-._
 *      LOG ME                              _.-~               ~-._
 *                                      _.-~                       ~---._
 *         LIKE ONE                 _.-~                                 ~\
 *                               .-~                                    _.;
 *            OF YOUR            :-._                               _.-~./
 *                               }-._~-._                   _..__.-~_.-~ )
 *               FRENCH          `-._~-._~-._              /   ...-~H.-~
 *                                   ~-._-.-_\.        _.-~ .::::  //
 *                  TOASTS               ~-. \`--...--~ _.-~__...==~
 *                                          \.`--...---+-~~~~~
 *                                            ~-..----~
 *
 *
 * The following macros can be used like printf(3) for logging purposes. FATAL
 * will also terminate the program.
 *
 *
 * The following environment variables are considered:
 *
 *   LOG_LEVEL          0 = OFF, 1 = ERROR, 2 = WARNING, 3 = INFO, 4 = DEBUG
 *
 *   LOG_FILE           Appends log output to a file instead of writing it to
 *                      stderr. If set to `-` stdout will be used.
 *
 *   LOG_LOCATION       Set to also output source location.
 *
 *   DISABLE_LOG_COLOR  Set to disable colorization of log output.
 *
 *
 * The following defines can be set during compilation:
 *
 *   DISABLE_LOG         Completely disables log output.
 *
 *   LOG_MULTI_THREADED  Locks a mutex for printing (not initialization).
 *                       Requires -pthread.
 *
 */

#define FATAL(f_, ...)    logger_log(LOG_LEVEL_ERROR,    LOG_COLOR_RED, true,  __FILE__, __LINE__, (f_), ##__VA_ARGS__)
#define ERROR(f_, ...)    logger_log(LOG_LEVEL_ERROR,    LOG_COLOR_RED, false, __FILE__, __LINE__, (f_), ##__VA_ARGS__)
#define WARNING(f_, ...)  logger_log(LOG_LEVEL_WARNING,  LOG_COLOR_YEL, false, __FILE__, __LINE__, (f_), ##__VA_ARGS__)
#define INFO(f_, ...)     logger_log(LOG_LEVEL_INFO,     LOG_COLOR_CYN, false, __FILE__, __LINE__, (f_), ##__VA_ARGS__)
#define DEBUG(f_, ...)    logger_log(LOG_LEVEL_DEBUG,    LOG_COLOR_BLU, false, __FILE__, __LINE__, (f_), ##__VA_ARGS__)

/* These macros are to be used like perror(3) with an argument. */

#define FATALno(tag_)   FATAL("%s: %s", (tag_), strerror(errno))
#define ERRORno(tag_)   ERROR("%s: %s", (tag_), strerror(errno))
#define WARNINGno(tag_) WARNING("%s: %s", (tag_), strerror(errno))
#define INFOno(tag_)    INFO("%s: %s", (tag_), strerror(errno))
#define DEBUGno(tag_)   DEBUG("%s: %s", (tag_), strerror(errno))

/* Sets the default log level. */

#define LOG_LEVEL_DEFAULT LOG_LEVEL_INFO

/* Implementation details follow. */

#ifdef DISABLE_LOG
    #undef FATAL
    #undef ERROR
    #undef WARNING
    #undef INFO
    #undef DEBUG

    #define FATAL(f_, ...)   exit(1);
    #define ERROR(f_, ...)   /* empty */
    #define WARNING(f_, ...) /* empty */
    #define INFO(f_, ...)    /* empty */
    #define DEBUG(f_, ...)   /* empty */
#endif

#define LOG_LEVEL_OFF     0
#define LOG_LEVEL_ERROR   1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_INFO    3
#define LOG_LEVEL_DEBUG   4

#define LOG_COLOR_NRM  "\x1B[0m"
#define LOG_COLOR_RED  "\x1B[31m"
#define LOG_COLOR_GRN  "\x1B[32m"
#define LOG_COLOR_YEL  "\x1B[33m"
#define LOG_COLOR_BLU  "\x1B[34m"
#define LOG_COLOR_MAG  "\x1B[35m"
#define LOG_COLOR_CYN  "\x1B[36m"
#define LOG_COLOR_WHT  "\x1B[37m"

void logger_log(int level, const char* color, bool terminate, const char* file,
                long line, const char* fmt, ...);

#endif
