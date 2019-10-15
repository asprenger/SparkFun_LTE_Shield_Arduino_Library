
#ifndef SPARKFUN_LTE_SHIELD_ARDUINO_LIBRARY_LOGGING_H
#define SPARKFUN_LTE_SHIELD_ARDUINO_LIBRARY_LOGGING_H

#define LOG_MAX_STRING_LENGTH 160

#define LOG(_level, _format, ...) \
  log(_level, _format, ##__VA_ARGS__);

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

#endif