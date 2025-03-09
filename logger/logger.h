#ifndef LOGGER_H
#define LOGGER_H

#ifndef LOGGER_FD
#define LOGGER_FD stdout
#endif // LOGGER_FD

// #define LOG()
#ifdef _WIN32
#define FILE_PATH_SEPRATOR '\\'
#else
#define FILE_PATH_SEPRATOR '/'
#endif
#define __FILENAME__ (strrchr(__FILE__, FILE_PATH_SEPRATOR) ? strrchr(__FILE__, FILE_PATH_SEPRATOR) + 1 : __FILE__)

typedef enum
{
    LOG_LEVEL_WARN,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG
} ELogLevel;

typedef struct
{
    int m_fd;
    ELogLevel level;
} SLoggerConfig;

// void LOG_initDefaultLogger(const ELogLevel logLevel);
// void LOG_log(...);

static const char* LOG_LEVEL_STR[] = {"WARN", "FATAL", "INFO", "DEBUG"};

#define LOG_HELPER(level, fmt, ...) fprintf(LOGGER_FD, "[%5s][%s:%d] "fmt "\n%s",LOG_LEVEL_STR[level], __FILENAME__, __LINE__, __VA_ARGS__)
#define LOG(level, ...) do { LOG_HELPER(level, __VA_ARGS__, ""); \
                             if(LOG_LEVEL_FATAL == level) assert(false && "FATAL exception!"); \
                             } while(0);

#endif // LOGGER_H