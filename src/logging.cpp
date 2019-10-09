
#include <logging.h>
#include <Arduino.h>

static LogLevel log_threshold = WARN;

void setLogLevel(LogLevel level) {
    log_threshold = level;
}

LogLevel getLogLevel() {
	return log_threshold;
}

const char* log_level_name(int level) {
    static const char* const names[] = {
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "PANIC"
    };
    return names[level]; // TODO DANGER!!!
}

void log_message_v(int level, const char *fmt, va_list args) {

  if (level < getLogLevel()) {
    return;
  }
  
  char buf[LOG_MAX_STRING_LENGTH];
  const unsigned long time = millis();
  const char* levelName = log_level_name(level);
  snprintf(buf, sizeof(buf), "%010u %s ", (unsigned)time, levelName);
  Serial.print(buf);
  
  const int n = vsnprintf(buf, sizeof(buf), fmt, args);
  if (n > (int)sizeof(buf) - 1) {
      buf[sizeof(buf) - 2] = '~';
  }
  Serial.println(buf);
}

void log(int level, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message_v(level, fmt, args);
    va_end(args);
}
