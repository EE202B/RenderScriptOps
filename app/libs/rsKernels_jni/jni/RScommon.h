#ifndef RSKERNELSTEST_RSCOMMON_H
#define RSKERNELSTEST_RSCOMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include "RenderScript.h"

using namespace android::RSC;

namespace androidrs {

sp<RS> mRS = new RS();
const char* cachePath = "/data/user/0/com.yingnanwang.rskernelstest/cache";

}

#endif //RSKERNELSTEST_RSCOMMON_H