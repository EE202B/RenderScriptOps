//
// Created by WangYingnan on 3/12/17.
//

#ifndef RSKERNELSTEST_RSMATMUL_H
#define RSKERNELSTEST_RSMATMUL_H

#include "common.h"
#include "RenderScript.h"

using namespace android::RSC;

void rsMatmul_sgemm(const char * path, void* a_ptr, bool a_trans, void* b_ptr, bool b_trans, void*& c_ptr,
                    int m, int n, int k, float alpha, float beta);
void rsMatmul_bnnm(const char * path, void* a_ptr, int a_off, void* b_ptr, int b_off, void*& c_ptr, int c_off,
                    int m, int n, int k, int c_mult);

#endif //RSKERNELSTEST_RSMATMUL_H
