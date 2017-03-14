//
// Created by WangYingnan on 3/12/17.
//

#ifndef RSKERNELSTEST_RSMATMUL_H
#define RSKERNELSTEST_RSMATMUL_H

#include "common.h"
#include "RenderScript.h"

using namespace android::RSC;

void rsMatmul_sgemm(const char * path, void* a_ptr, void* b_ptr, void*& c_ptr, int m, int n, int k);
void rsMatmul_bnnm(const char * path, void* a_ptr, void* b_ptr, void*& c_ptr, int m, int n, int k);

#endif //RSKERNELSTEST_RSMATMUL_H
