#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "logger.h"

static const char *level_strings[] = 
{
  "TRACE", 
  "DEBUG",
  "INFO",
  "WARN",
  "ERROR"
};

static const char *level_colors[] =
{
  "\x1b[94m", 
  "\x1b[36m",
  "\x1b[32m", 
  "\x1b[33m",
  "\x1b[31m" 
};

static struct
{
    void *stream;
    bool colorize;
    log_level_t level;
} logger_glob;

bool is_active(log_level_t level)
{
    return level >= logger_glob.level;
}

int init_logger(void *stream,
                log_level_t level,
                bool colorize)
{
    logger_glob.level = level;
    logger_glob.colorize = colorize;
    logger_glob.stream = stream;
    return stream ? 0 : 1;
}

void log_msg(log_level_t level, 
             const char *file,
             int line,
             const char *fmt, ...)
{
    if (! is_active(level))
    {
        return;
    }

    char ts[16];
    time_t t = time(NULL);
    ts[strftime(ts, sizeof(ts), "%H:%M:%S", localtime(&t))] = '\0';

    va_list ap;
    va_start(ap, fmt);
    
    if (logger_glob.colorize)
    {
        fprintf(stderr,
                "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
                ts,
                level_colors[level],
                level_strings[level],
                file,
                line);
    }
    else
    {
        fprintf(stderr,
                "%s %-5s %s:%d: ",
                ts,
                level_strings[level],
                file,
                line);        
    }
    vfprintf(stderr,
             fmt,
             ap);
    fprintf(stderr, "\n");
    fflush(stderr);

    va_end(ap);
}
