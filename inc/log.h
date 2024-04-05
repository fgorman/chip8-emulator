#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define error_log(fmt, ...) { \
    fprintf(stderr, "[ERROR] %s:%s:%d : ", __FILE__, __func__, __LINE__); \
    fprintf(stderr, fmt, ##__VA_ARGS__); \
}

#endif 