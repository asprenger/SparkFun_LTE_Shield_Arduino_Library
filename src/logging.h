
#ifndef LOG_MAX_STRING_LENGTH
#define LOG_MAX_STRING_LENGTH 160
#endif

#define LOG(_level, _format, ...) \
  log(_level, _format, __VA_ARGS__);

typedef enum LogLevel {
        DEBUG = 0,
        INFO = 1,
        WARN = 2,
        ERROR = 3,
        PANIC = 4
} LogLevel;

void setLogLevel(LogLevel level);
LogLevel getLogLevel();
void log(int level, const char *fmt, ...);
