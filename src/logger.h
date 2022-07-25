#if !defined(LOGGER_H)
#define LOGGER_H

#include <stdbool.h>

enum log_level
{
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
};
typedef enum log_level log_level_t;

int init_logger(void *stream,
                log_level_t level,
                bool colorize);
void log_msg(log_level_t level, 
             const char *file,
             int line,
             const char *fmt, ...);

#define log_trace(...) log_msg(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_msg(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_msg(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_msg(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_msg(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#endif