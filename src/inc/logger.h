#pragma once
#include <stdio.h>

#define LOG_LEVEL_INFO 0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_ERROR 3

#define printd(LOG_LEVEL, fmt) \
    do{\
        if (LOG_LEVEL >= LOG_LEVEL){\
            if(LOG_LEVEL == LOG_LEVEL_DEBUG){\
                fprintf(stderr, "DEBUG:\t");\
            }\
            if(LOG_LEVEL == LOG_LEVEL_INFO){\
                fprintf(stderr, "INFO:\t");\
            }\
            if(LOG_LEVEL == LOG_LEVEL_WARN){\
                fprintf(stderr, "WARN:\t");\
            }\
            if(LOG_LEVEL == LOG_LEVEL_ERROR){\
                fprintf(stderr, "ERROR:\t");\
            }\
            fprintf(stderr, "%s:%s:%d:%s():\n", fmt, __FILE__, __LINE__, __func__);\
        }\
    }while(0)
