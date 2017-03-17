//
// Created by WangYingnan on 3/12/17.
//

#ifndef RSKERNELSTEST_RSCONV_H
#define RSKERNELSTEST_RSCONV_H

#include "common.h"
#include <vector>
#include <memory>
#include "RenderScript.h"

using namespace android::RSC;

struct rsConvInfo{
    int in_depth;
    int input_rows;
    int input_cols;

    int filter_rows;
    int filter_cols;

    int stride_rows;
    int stride_cols;

    int pad_rows;
    int pad_cols;

    int out_depth;
    int out_rows;
    int out_cols;

    int batch;
    int data_format; // 0 F32, 1 U8
    int conv_stride;

    rsConvInfo(int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10, int n11, int n12, int n13, int n14, int n15){
        in_depth=n1;
        input_rows=n2+n8*2;
        input_cols=n3+n9*2;
        filter_rows=n4;filter_cols=n5;
        stride_rows=n6;stride_cols=n7;
        pad_rows=n8;pad_cols=n9;
        out_depth=n10;out_rows=n11;out_cols=n12;
        batch=n13;data_format=n14;conv_stride=n15;
    };
};

void rsConv3_3_float_intrinsic(const char * path, void* filter, void* input, void*& output, rsConvInfo convInfo);


#endif //RSKERNELSTEST_RSCONV_H