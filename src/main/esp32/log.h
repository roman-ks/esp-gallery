#pragma once
#include <Arduino.h>
#include <typeinfo>

// #define LOGF(fmt, ...) \
//     do { \
//         Serial.printf("[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
//     } while

#define LOG(msg) \
    do { \
        Serial.printf("[%s:%d] " msg "\n", __FILE__, __LINE__); \
    } while(0)