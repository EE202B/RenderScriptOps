/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <jni.h>
#include <unistd.h>

#include "common.h"
#include "timer.h"
#include "rsMatmul_test.h"
#include "rsConv_test.h"

extern "C" JNIEXPORT void JNICALL
Java_com_yingnanwang_rskernelstest_RSJniInterface_sgemmTest(JNIEnv * env,
                                                       jclass,
                                                       jstring pathObj
                                                       )
{
    // convert the jni to c++ type
    const char * path = env->GetStringUTFChars(pathObj, nullptr);

    Timer t;

    LOGD("+-+-+-+-Matmul Float test-+-+-+-+");
    androidrs::matmul::smallTest<float>(path);
    androidrs::matmul::mediumTest<float>(path);
    androidrs::matmul::largeTest<float>(path);

    LOGD("+-+-+-+-Matmul uint8_t test-+-+-+-+");
    androidrs::matmul::smallTest<uint8_t>(path);
    androidrs::matmul::mediumTest<uint8_t>(path);
    androidrs::matmul::largeTest<uint8_t>(path);
    
    LOGD("+-+-+-+-Conv Float test-+-+-+-+");
    androidrs::conv::smallTest<float>(path);
    androidrs::conv::largeTest_F32<float>(path);

    LOGD("+-+-+-+-Conv uint8_t test-+-+-+-+");
    // androidrs::conv::largeTest_U8<uint8_t>(path);

    t.print();

    //release the resource
    env->ReleaseStringUTFChars(pathObj, path);
}
