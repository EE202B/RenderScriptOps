#ifndef RSKERNELSTEST_RSCONV_TEST_H
#define RSKERNELSTEST_RSCONV_TEST_H

#include "rsConv.h"

namespace androidrs {
namespace conv {

template <typename T>
float calcL2Norm(T* input, int sz)
{
    float l2Norm = 0;
    for (int i = 0; i < sz; ++i) {
        l2Norm += (float)input[i] * input[i];
    }
    return l2Norm;
}

template <typename T>
bool testWithTolerance(void* out, void* ref, int sz)
{
    T* casted_out = static_cast<T*>(out);
    T* casted_ref = static_cast<T*>(ref);

    float l2NormOut = calcL2Norm(casted_out, sz);
    float l2NormRef = calcL2Norm(casted_ref, sz);

    float tolerance = ALLOWED_ERROR * (l2NormOut < l2NormRef ? l2NormOut : l2NormRef);
    tolerance /= sz;

    for (int i = 0; i < sz; ++i) {
        float err = casted_out[i] - casted_ref[i];
        float absErr = err * err;
        if (absErr > tolerance) {
            return false;
        }
    }
    return true;
}

template <typename T>
void smallTest_rsConv3_3(const char * path, bool isValid)
{
    //intrinsic & myScript
    T filters[] = {
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
    T input[] = {
        0, 1, 2, 0, 0, 0, 1, 2, 1, 1, 2, 2, 2, 1, 2,
        2, 2, 0, 2, 2, 2, 1, 2, 1, 2, 2, 1, 1, 2, 0,
        0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 2, 2, 0, 1, 1,
        2, 2, 1, 0, 0, 1, 2, 0, 0, 2, 0, 1, 0, 1, 1,
        2, 1, 1, 0, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 0,
    };
    T output_ref[] = {
        9, -1, 13, 5, 13, 7, 2, -8, 2, -13, 6, -3, 1, -5, 2, -4, 3, -4,
    };
    // T filters[] = {
    //     0, 0, 1, -1, -1, -1,
    //     -1, 1, -1, 1, -1, -1,
    //     -1, 1, -1, -1, 1, 1,
    //     -1, 1, 0, 0, -1, -1,
    //     1, 1, -1, 0, 0, 1,
    //     -1, 1, 1, -1, 0, 0,
    //     1, 0, 0, 1, 1, 1,
    //     -1, -1, -1, 1, 1, 1,
    //     1, 1, -1, 0, 1, -1,
    // };
    // T input[] = {
    //     2, 2, 1, 1, 1, 1, 2, 0, 0, 0, 2, 2, 2, 1, 1,
    //     1, 0, 2, 2, 0, 1, 2, 0, 1, 0, 0, 2, 1, 0, 0,
    //     0, 0, 2, 2, 1, 0, 0, 1, 0, 1, 1, 2, 1, 0, 2,
    //     2, 1, 2, 1, 2, 2, 1, 0, 0, 0, 0, 0, 1, 0, 1,
    //     2, 2, 1, 1, 1, 0, 2, 0, 0, 1, 0, 2, 1, 2, 2,
    // };
    // T output_ref[] = {
    //     4, 5, 6, -2, 0, 2, -5, 5, -3, 7, -5, 1, -6, 5, -1, 1, -6, 2,
    // };

    void* output = new T[18];
    rsConvInfo smallConvInfo(3, 5, 5, 3, 3, 2, 2, 1, 1, 2, 3, 3, 1, sizeof(T));

    rsConv_intrinsic<T>(path, static_cast<void*>(filters), static_cast<void*>(input), output, smallConvInfo);
    if(!testWithTolerance<T>(output, static_cast<void*>(output_ref), 18)){
        LOGE("rsConv_intrinsic 3x3 small test failed!");
    }else{
        LOGI("rsConv_intrinsic 3x3 small test passed!");
    }

    rsConv_script<T>(path, static_cast<void*>(filters), static_cast<void*>(input), output, smallConvInfo);
    if(!testWithTolerance<T>(output, static_cast<void*>(output_ref), 18)){
        LOGE("rsConv_script 3x3 small test failed!");
    }else{
        LOGI("rsConv_script 3x3 small test passed!");
    }

    // for(int i=0;i<18;++i){
    //     LOGI("%f", static_cast<T*>(output)[i]);
    // }

    delete[] static_cast<T*>(output);
}

void largeTest_rsConv3_3(const char * path, bool isValid)
{
    //intrinsic & myScript
}

void largeTest_rsConv5_5(const char * path, bool isValid)
{
    //intrinsic & myScript
}

void dummyTest(const char * path)
{
    float kernel[] = {
            1, 2, 1,
            0, 0, 0,
            -1, -2, -1,
    };
    float input[] = {
            0, 0, 0, 0, 0,
            0, 1, 2, 3, 0,
            0, 4, 5, 6, 0,
            0, 7, 8, 9, 0,
            0, 0, 0, 0, 0,
    };
    float input2[] = {
            0, 0, 0, 0, 0,
            0, 100, 200, 300, 0,
            0, 400, 500, 600, 0,
            0, 700, 800, 900, 0,
            0, 0, 0, 0, 0,
    };
    size_t k = 5;

    sp<RS> rs = new RS();
    rs->init(path);
    sp<const Element> e = Element::F32(rs);

    sp<const Type> input_t = Type::create(rs, e, k, k, 0);
    sp<const Type> output_t = Type::create(rs, e, k, k, 0);

    sp<Allocation> input_alloc = Allocation::createTyped(rs, input_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> output_alloc = Allocation::createTyped(rs, output_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    input_alloc->copy2DRangeFrom(0, 0, k, k, static_cast<void*>(input));

    sp<Allocation> input2_alloc = Allocation::createTyped(rs, input_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> output2_alloc = Allocation::createTyped(rs, output_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    input2_alloc->copy2DRangeFrom(0, 0, k, k, static_cast<void*>(input2));

    sp<ScriptIntrinsicConvolve3x3> sc = ScriptIntrinsicConvolve3x3::create(rs, e);
    
    sc->setCoefficients(kernel);
    sc->setInput(input_alloc);
    sc->forEach(output_alloc);

    sc->setCoefficients(kernel);
    sc->setInput(input2_alloc);
    sc->forEach(output2_alloc);

    void* out = new float[k*k];
    output_alloc->copy2DRangeTo(0, 0, k, k, out);

    void* out2 = new float[k*k];
    output2_alloc->copy2DRangeTo(0, 0, k, k, out2);

    for(int i=0;i<k*k;++i){
        LOGI("%f", static_cast<float*>(out)[i]);
    }
    LOGE("Second rs");
    for(int i=0;i<k*k;++i){
        LOGI("%f", static_cast<float*>(out2)[i]);
    }

    delete[] static_cast<float*>(out);
    delete[] static_cast<float*>(out2);
}

void scriptTestFloat(const char * path)
{
    float input[] = {
            0, 0, 0, 0, 0,
            0, 1, 2, 3, 0,
            0, 4, 5, 6, 0,
            0, 7, 8, 9, 0,
            0, 0, 0, 0, 0,
    };
    size_t k = 5;

    sp<RS> rs = new RS();
    rs->init(path);
    sp<const Element> e = Element::F32(rs);

    sp<const Type> data_t = Type::create(rs, e, k, k, 0);

    sp<Allocation> input_alloc = Allocation::createTyped(rs, data_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> output_alloc = Allocation::createTyped(rs, data_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);

    input_alloc->copy2DRangeFrom(0, 0, k, k, static_cast<void*>(input));

    ScriptC_utils* sc = new ScriptC_utils(rs);
    sc->forEach_sumAlloc_F32(input_alloc, output_alloc);
    rs->finish();

    void* out = new float[k*k];
    output_alloc->copy2DRangeTo(0, 0, k, k, out);

    for(int i=0;i<k*k;++i){
        LOGI("%f", static_cast<float*>(out)[i]);
    }

    delete[] static_cast<float*>(out);
}

}
}

#endif