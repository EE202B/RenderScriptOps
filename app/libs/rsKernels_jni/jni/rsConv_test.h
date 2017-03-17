#ifndef RSKERNELSTEST_RSCONV_TEST_H
#define RSKERNELSTEST_RSCONV_TEST_H

#include "rsConv.h"

namespace rsconvtest {

namespace conv3 {

#define DATA_TYPE float

DATA_TYPE calcL2Norm(DATA_TYPE* input, int sz)
{
    float l2Norm = 0.f;
    for (int i = 0; i < sz; ++i) {
        l2Norm += input[i] * input[i];
    }
    return l2Norm;
}

bool testWithTolerance(void* out, void* ref, int sz)
{
    DATA_TYPE* casted_out = static_cast<DATA_TYPE*>(out);
    DATA_TYPE* casted_ref = static_cast<DATA_TYPE*>(ref);

    DATA_TYPE l2NormOut = calcL2Norm(casted_out, sz);
    DATA_TYPE l2NormRef = calcL2Norm(casted_ref, sz);

    DATA_TYPE tolerance = ALLOWED_ERROR * (l2NormOut < l2NormRef ? l2NormOut : l2NormRef);
    tolerance /= m * n;

    for (int i = 0; i < m*n; ++i) {
        DATA_TYPE err = casted_out[i] - casted_ref[i];
        DATA_TYPE absErr = err * err;
        if (absErr > tolerance) {
            return false;
        }
    }
    return true;
}

void SmallTest(const char * path, bool isValid)
{
    float filters[] = {
        0, -1, 0, 0, -1, 0, 
        -1, -1, 0, -1, 0, -1,
        0, 0, -1, 0, 0, -1,
        1, -1, 0, 0, 1, 0,
        0, 1, 1, 1, 1, -1, 
        0, -1, 0, -1, 1, 0, 
        1, 1, 1, 1, 0, 1,
        1, 0, 1, 1, 0, -1,
        0, -1, 0, 1, 1, -1,
    };
    float input[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 2, 0, 0, 0, 1, 2, 1, 1, 2, 2, 2, 1, 2, 0, 0, 0,
        0, 0, 0, 2, 2, 0, 2, 2, 2, 1, 2, 1, 2, 2, 1, 1, 2, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 2, 2, 0, 1, 1, 0, 0, 0,
        0, 0, 0, 2, 2, 1, 0, 0, 1, 2, 0, 0, 2, 0, 1, 0, 1, 1, 0, 0, 0,
        0, 0, 0, 2, 1, 1, 0, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    float output_ref[] = {
        9, -1, 13, 5, 13, 7, 2, -8, 2, -13, 6, -3, 1, -5, 2, -4, 3, -4,
    };
    rsConvInfo smallConvInfo(3, 5, 5, 3, 3, 1, 1, 1, 1, 2, 3, 3, 1, 0, 2);

    void* output = new float[18];
    rsConv3_3_float_intrinsic(path, static_cast<void*>(filters), static_cast<void*>(input), output, smallConvInfo);

    if(testWithTolerance(output, static_cast<void*>(output_ref), 18)){
        LOGE("rsConv3_3_float_intrinsic small test failed!");
    }else{
        LOGI("rsConv3_3_float_intrinsic small test passed!");
    }

    delete[] static_cast<float*>(output);
}

}
}

#endif