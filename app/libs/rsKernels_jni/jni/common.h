//
// Created by WangYingnan on 3/12/17.
//

#ifndef RSKERNELSTEST_ANDROIDLOG_H
#define RSKERNELSTEST_ANDROIDLOG_H

#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>


#define DATA_TYPE float
#define ALLOWED_ERROR 0.000001f

#define LOG_TAG "NDK_LOG"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


#endif //RSKERNELSTEST_ANDROIDLOG_H
