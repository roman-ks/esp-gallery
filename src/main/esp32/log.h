#pragma once
#include <Arduino.h>
#include <typeinfo>
#include <cstring>

#ifndef LOG_LEVEL
#define LOG_LEVEL 0 // 0 - TRACE, 1 - DEBUG, 2 - INFO, 3 - WARN, 4 - ERROR
#endif

inline const char* normalizePath(const char* path){
    static char buf[512];
    size_t len = strlen(path) + 1;
    if(len >= sizeof(buf)) len = sizeof(buf)-1;
    for(size_t i=0; i<len; i++){
        buf[i] = path[i];
        if(buf[i] == '\\') buf[i] = '/';
    }
    return buf;
}

#define FILE_RELATIVE(file) \
    (strstr(normalizePath(file), "/src/") ? (strstr(normalizePath(file), "/src/") + 5) : file)

inline const char* createLoggerName(char *file, long line){
    static char buf[512];
    sprintf(buf, "[%s:%d]", FILE_RELATIVE(file), line);
    return buf;
}


#define LOGF(fmt, ...) \
    do { \
        Serial.printf("%-40s " fmt, createLoggerName(__FILE__, __LINE__), ##__VA_ARGS__); \
    } while(0)

#define LOG(msg) \
    do { \
        Serial.printf("%-40s " msg "\n", createLoggerName(__FILE__, __LINE__)); \
    } while(0)

#if LOG_LEVEL <=1 
#define LOGF_D(fmt, ...) \
    do { \
        Serial.printf("%-40s [DEBUG] " fmt, createLoggerName(__FILE__, __LINE__), ##__VA_ARGS__); \
    } while(0)
#define LOG_D(msg) \
    do { \
        Serial.printf("%-40s [DEBUG] " msg "\n", createLoggerName(__FILE__, __LINE__)); \
    } while(0)
    
#define LOGF_I(fmt, ...) \
    do { \
        Serial.printf("%-40s [INFO ] " fmt, createLoggerName(__FILE__, __LINE__), ##__VA_ARGS__); \
    } while(0)

#define LOG_I(msg) \
    do { \
        Serial.printf("%-40s [INFO ] " msg "\n", createLoggerName(__FILE__, __LINE__)); \
    } while(0)
#elif LOG_LEVEL == 2 
#define LOGF_D(fmt, ...)
#define LOG_D(msg)
#define LOGF_I(fmt, ...) \
    do { \
        Serial.printf("[INFO ] %-40s " fmt, createLoggerName(__FILE__, __LINE__), ##__VA_ARGS__); \
    } while(0)

#define LOG_I(msg) \
    do { \
        Serial.printf("[INFO ] %-40s " msg "\n", createLoggerName(__FILE__, __LINE__)); \
    } while(0)
#endif