//
// Created by WangYingnan on 3/12/17.
//

#ifndef RSKERNELSTEST_RSMATMUL_TEST_H
#define RSKERNELSTEST_RSMATMUL_TEST_H

#include "rsMatmul.h"
#include "rsMatmul_test_data.h"
#include "rsQuan_test_data.h"

#define ALLOWED_ERROR 0.000001f

namespace androidrs {

namespace matmul {

template <typename T>
void getLargeData(T*& a, T*& b, int m, int n, int k)
{
    auto a_raw = getA();
    auto b_raw = getB();

    a = new T[m*k];
    b = new T[n*k];

    for(int i=0;i<m*k;++i){
        a[i] = (T)a_raw[i];
    }
    for(int i=0;i<n*k;++i){
        b[i] = (T)b_raw[i];
    }
}

template <typename T>
void getRefResult(T* a, T* b, void*& c, int m, int n, int k)
{
    c = new T[m*n];
    T* c_casted = static_cast<T*>(c);

    if(sizeof(T)==4){
        for (int j = 0; j < n; j++) {
            for (int i = 0; i < m; i++) {
                T total = 0;
                for (int l = 0; l < k; l++) {
                    int a_index = ((i * k) + l);
                    int b_index = ((l * n) + j);
                    T mult = a[a_index] * b[b_index];
                    total += mult;
                }
                int c_index = ((i * n) + j);
                c_casted[c_index] = total;
            }
        }
    }else if(sizeof(T)==1){
        for(int j = 0; j < n; ++j){
            for(int i = 0; i < m; ++i){
                T total = 0;
                for (int l = 0; l < k; l++) {
                    int a_index = ((i * k) + l);
                    int b_index = ((j * k) + l);
                    T mult = a[a_index] * b[b_index];
                    total += mult;
                }
                int c_index = ((i * n) + j);
                c_casted[c_index] = total;
            }
        }
    }
}

template <typename T>
float calcL2Norm(T* input, int sz)
{
    float l2Norm = 0.f;
    for (int i = 0; i < sz; ++i) {
        l2Norm += input[i] * input[i];
    }
    return l2Norm;
}

template <typename T>
bool testWithTolerance(void* out, void* ref, int m, int n)
{
    T* casted_out = static_cast<T*>(out);
    T* casted_ref = static_cast<T*>(ref);

    float l2NormOut = calcL2Norm<T>(casted_out, m*n);
    float l2NormRef = calcL2Norm<T>(casted_ref, m*n);

    float tolerance = ALLOWED_ERROR * (l2NormOut < l2NormRef ? l2NormOut : l2NormRef);
    tolerance /= m * n;

    for (int i = 0; i < m*n; ++i) {
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
    int m=2, n=4, k=3;
    int a_sz = m*k, b_sz = n*k, c_sz = m*n;
    T* a_ori = new T[a_sz];
    T* b_ori = new T[b_sz];

    T a_data[] = {
                1, 2, 3,
                4, 5, 6,
    };
    T b_data[] = {
                11, 7, 3,
                10, 6, 2,
                9, 5, 1,
                8, 4, 0,
    };

    for(int i=0;i<a_sz;++i){
        a_ori[i] = a_data[i];
    }
    for(int i=0;i<b_sz;++i){
        b_ori[i] = b_data[i];
    }

    void *c_out = new T[m*n];
    if(sizeof(T)==4){
        rsMatmul_sgemm(static_cast<void*>(a_ori), false, static_cast<void*>(b_ori), false, c_out, m, n, k, 1, 0);
    }else if(sizeof(T)==1){
        rsMatmul_bnnm(static_cast<void*>(a_ori), 0, static_cast<void*>(b_ori), 0, c_out, 0, m, n, k, (1<<21));
    }

    void* c_ref;
    getRefResult<T>(a_ori, b_ori, c_ref, m, n, k);

    if(!testWithTolerance<T>(c_out, c_ref, m, n)){
        LOGE("rsMatmul small test failed!");
    }else{
        LOGI("rsMatmul small test passed!");
    }

    delete[] a_ori;
    delete[] b_ori;
    delete[] static_cast<T*>(c_out);
    delete[] static_cast<T*>(c_ref);
}


template <typename T>
void mediumTest(const char * path)
{
    int m=7, n=9, k=23;
    int a_sz = m*k, b_sz = n*k, c_sz = m*n;
    T* a_ori = new T[a_sz];
    T* b_ori = new T[b_sz];

    T a_data[] = {
                1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
                1, 23, 2, 22, 3, 21, 4, 20, 5, 19, 6, 18, 7, 17, 8, 16, 9, 15, 10, 14, 11, 13, 12,
                23, 1, 22, 2, 21, 3, 20, 4, 19, 5, 18, 6, 17, 7, 16, 8, 15, 9, 14, 10, 13, 11, 12,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
                3, 1, 4, 1, 5, 8, 2, 3, 1, 14, 11, 15, 18, 12, 13, 11, 14, 11, 15, 18, 12, 13, 11,
                8, 0, 5, 8, 1, 3, 7, 5, 7, 13, 10, 23, 13, 11, 17, 23, 12, 19, 17, 13, 14, 10, 19,
    };
    T b_data[] = {
                0, 2, 4, 6, 8, 10, 1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10, 1, 3, 5, 7, 9,
                0, 20, 40, 60, 80, 10, 11, 13, 15, 17, 19, 21, 10, 12, 14, 6, 8, 10, 1, 3, 5, 7, 9,
                1, 21, 41, 61, 81, 11, 12, 14, 16, 18, 20, 22, 11, 13, 15, 7, 9, 11, 2, 4, 6, 8, 9,
                0, 19, 39, 59, 79, 9, 10, 12, 14, 16, 18, 20, 9, 11, 13, 5, 7, 9, 0, 2, 4, 6, 8,
                2, 22, 42, 62, 82, 12, 13, 15, 17, 19, 21, 23, 12, 14, 16, 8, 9, 12, 3, 5, 7, 9, 9,
                0, 18, 38, 58, 78, 8, 9, 11, 13, 15, 17, 19, 8, 10, 12, 4, 6, 8, 0, 1, 3, 5, 7,
                3, 23, 43, 63, 83, 13, 14, 16, 18, 20, 22, 24, 13, 15, 17, 9, 9, 13, 4, 6, 8, 9, 9,
                0, 17, 37, 57, 77, 7, 8, 10, 12, 14, 16, 18, 7, 9, 11, 3, 5, 7, 0, 0, 2, 4, 6,
                10, 20, 30, 40, 50, 1, 2, 3, 4, 5, 11, 12, 13, 14, 15, 21, 22, 23, 24, 25, 1, 2, 3,
    };

    for(int i=0;i<a_sz;++i){
        a_ori[i] = a_data[i];
    }
    for(int i=0;i<b_sz;++i){
        b_ori[i] = b_data[i];
    }

    void *c_out = new T[m*n];
    if(sizeof(T)==4){
        rsMatmul_sgemm(static_cast<void*>(a_ori), false, static_cast<void*>(b_ori), false, c_out, m, n, k, 1, 0);
    }else if(sizeof(T)==1){
        rsMatmul_bnnm(static_cast<void*>(a_ori), 13, static_cast<void*>(b_ori), 23, c_out, 2121, m, n, k, 132359);
    }

    void* c_ref;
    getRefResult<T>(a_ori, b_ori, c_ref, m, n, k);

    if(!testWithTolerance<T>(c_out, c_ref, m, n)){
        LOGE("rsMatmul medium test failed!");
    }else{
        LOGI("rsMatmul medium test passed!");
    }

    delete[] a_ori;
    delete[] b_ori;
    delete[] static_cast<T*>(c_out);
    delete[] static_cast<T*>(c_ref);
}

template <typename T>
void largeTest(const char * path)
{
    int m=256, n=192, k=1152;
    int a_sz = m*k, b_sz = n*k, c_sz = m*n;
    T *a_ori, *b_ori;
    getLargeData<T>(a_ori, b_ori, m, n, k);

    void *c_out = new T[m*n];
    if(sizeof(T)==4){
        rsMatmul_sgemm(static_cast<void*>(a_ori), false, static_cast<void*>(b_ori), false, c_out, m, n, k, 1, 0);
    }else if(sizeof(T)==1){
        rsMatmul_bnnm(static_cast<void*>(a_ori), 0, static_cast<void*>(b_ori), 84, c_out, 74980, m, n, k, 3401);
    }

    void* c_ref;
    getRefResult<T>(a_ori, b_ori, c_ref, m, n, k);

    if(!testWithTolerance<T>(c_out, c_ref, m, n)){
        LOGE("rsMatmul large test failed!");
    }else{
        LOGI("rsMatmul large test passed!");
    }

    delete[] a_ori;
    delete[] b_ori;
    delete[] static_cast<T*>(c_out);
    delete[] static_cast<T*>(c_ref);
}

template <typename T>
void tfTest_F32(const char * path)
{
    {
        auto A = getTFA();
        auto B = getTFB();
        auto c_ref = getTFC();
        const int m = 1;
        const int n = 1008;
        const int k = 1024;
        const int outsz = m*n;

        void* c_out = new T[outsz];
        if(sizeof(T)==4){
            rsMatmul_sgemm(static_cast<void*>(A), false, static_cast<void*>(B), false, c_out, m, n, k, 1, 0);
        }

        if(!testWithTolerance<T>(c_out, c_ref, m, n)){
            LOGE("rsMatmul float TF test failed!");
        }else{
            LOGI("rsMatmul float TF test passed!");
        }

        delete[] static_cast<T*>(c_out);
    }
}

// not implemented due to type mismatch
// gemmlowp::QuantizedGemm is int32_t while RS::BNNM output type is uint8_t
template <typename Tin, typename Tout>
void tfTest_U8(const char * path)
{
    {
        auto A = getTFQuanA();
        auto B = getTFQuanB();
        auto c_ref = getTFQuanC();
        const int m = 1;
        const int n = 1008;
        const int k = 1024;
        const int outsz = m*n;

        void* c_out = new Tin[outsz];
        if(sizeof(Tin)==1){
            rsMatmul_bnnm(static_cast<void*>(A), 173, static_cast<void*>(B), 139, c_out, 0, m, n, k, 1);
        }

        if(!testWithTolerance<Tin>(c_out, c_ref, m, n)){
            LOGE("rsMatmul uint8_t TF test failed!");
        }else{
            LOGI("rsMatmul uint8_t TF test passed!");
        }

        delete[] static_cast<Tout*>(c_out);
    }
}

}
}

#endif //RSKERNELSTEST_RSMATMUL_TEST_H
