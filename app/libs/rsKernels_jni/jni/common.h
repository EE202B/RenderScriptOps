//
// Created by WangYingnan on 3/12/17.
//

#ifndef RSKERNELSTEST_COMMON_H
#define RSKERNELSTEST_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>

#define LOG_TAG "NDK_LOG"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


#endif //RSKERNELSTEST_COMMON_H
