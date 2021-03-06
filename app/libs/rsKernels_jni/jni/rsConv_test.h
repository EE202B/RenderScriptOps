#ifndef RSKERNELSTEST_RSCONV_TEST_H
#define RSKERNELSTEST_RSCONV_TEST_H

#include "rsConv.h"
#include "rsConv_test_data.h"

#define ALLOWED_ERROR 0.000001f

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
void smallTest(const char * path)
{
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

    rsConv_intrinsic<T>(static_cast<void*>(filters), static_cast<void*>(input), output, smallConvInfo);
    if(!testWithTolerance<T>(output, static_cast<void*>(output_ref), 18)){
        LOGE("rsConv_intrinsic 3x3 float small test failed!");
    }else{
        LOGI("rsConv_intrinsic 3x3 float small test passed!");
    }

    // rsConv_script<T>(static_cast<void*>(filters), static_cast<void*>(input), output, smallConvInfo);
    // if(!testWithTolerance<T>(output, static_cast<void*>(output_ref), 18)){
    //     LOGE("rsConv_script small test failed!");
    // }else{
    //     LOGI("rsConv_script small test passed!");
    // }

    // for(int i=0;i<18;++i){
    //     LOGI("%f", static_cast<T*>(output)[i]);
    // }

    delete[] static_cast<T*>(output);
}

template <typename T>
void tfTest_F32(const char * path)
{

    ///////////////////////////////////////////////
    // conv 1 test
    {
        auto input = getConv1_input_F32();
        auto filter = getConv1_filter_F32();
        auto output_ref = getConv1_output_F32();
        const int outputSz = 2048;
        rsConvInfo convInfo(508, 4, 4, 1, 1, 1, 1, 0, 0, 128, 4, 4, 1, sizeof(T));

        void* output = new T[outputSz];
        rsConv_script<T>(static_cast<void*>(filter), static_cast<void*>(input), output, convInfo);
        if(!testWithTolerance<T>(output, static_cast<void*>(output_ref), outputSz)){
            LOGE("rsConv_script 1x1 float TF test failed!");
        }else{
            LOGI("rsConv_script 1x1 float TF test passed!");
        }
        delete[] static_cast<T*>(output);
    }
    ///////////////////////////////////////////////

    ///////////////////////////////////////////////
    // conv 3 test
    {
        auto input = getConv3_input_F32();
        auto filter = getConv3_filter_F32();
        auto output_ref = getConv3_output_F32();
        const int outputSz = 602112;
        rsConvInfo convInfo(64, 56, 56, 3, 3, 1, 1, 1, 1, 192, 56, 56, 1, sizeof(T));

        void* output = new T[outputSz];
        rsConv_script<T>(static_cast<void*>(filter), static_cast<void*>(input), output, convInfo);
        if(!testWithTolerance<T>(output, static_cast<void*>(output_ref), outputSz)){
            LOGE("rsConv_script 3x3 float TF test failed!");
        }else{
            LOGI("rsConv_script 3x3 float TF test passed!");
        }
        delete[] static_cast<T*>(output);
    }
    ///////////////////////////////////////////////

    ///////////////////////////////////////////////
    // conv 5 test
    {
        auto input = getConv5_input_F32();
        auto filter = getConv5_filter_F32();
        auto output_ref = getConv5_output_F32();
        const int outputSz = 25088;
        rsConvInfo convInfo(16, 28, 28, 5, 5, 1, 1, 2, 2, 32, 28, 28, 1, sizeof(T));

        void* output = new T[outputSz];
        rsConv_script<T>(static_cast<void*>(filter), static_cast<void*>(input), output, convInfo);
        if(!testWithTolerance<T>(output, static_cast<void*>(output_ref), outputSz)){
            LOGE("rsConv_script 5x5 float TF test failed!");
        }else{
            LOGI("rsConv_script 5x5 float TF test passed!");
        }
        delete[] static_cast<T*>(output);
    }
    ///////////////////////////////////////////////

    ///////////////////////////////////////////////
    // conv 7 test
    {
        auto input = getConv7_input_F32();
        auto filter = getConv7_filter_F32();
        auto output_ref = getConv7_output_F32();
        const int outputSz = 802816;
        rsConvInfo convInfo(3, 224, 224, 7, 7, 2, 2, 2, 2, 64, 112, 112, 1, sizeof(T));

        void* output = new T[outputSz];
        rsConv_script<T>(static_cast<void*>(filter), static_cast<void*>(input), output, convInfo);
        if(!testWithTolerance<T>(output, static_cast<void*>(output_ref), outputSz)){
            LOGE("rsConv_script 7x7 float TF test failed!");
        }else{
            LOGI("rsConv_script 7x7 float TF test passed!");
        }
        delete[] static_cast<T*>(output);
    }
    ///////////////////////////////////////////////
}

void largeTest_rsConv_U8(const char * path)
{

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