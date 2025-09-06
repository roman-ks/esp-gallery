#pragma once
#include <Arduino.h>
#include <typeinfo>
#include <cstring>

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