#pragma once
#include <Arduino.h>
#include <typeinfo>
#include <cstring>

inline const char* normalizePath(const char* path){
    static char buf[256];
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

#define LOGF(fmt, ...) \
    do { \
        Serial.printf("[%s:%d] " fmt, FILE_RELATIVE(__FILE__), __LINE__, ##__VA_ARGS__); \
    } while(0)

#define LOG(msg) \
    do { \
        Serial.printf("[%s:%d] " msg "\n", FILE_RELATIVE(__FILE__), __LINE__); \
    } while(0)