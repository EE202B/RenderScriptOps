//
// Created by WangYingnan on 3/12/17.
//

#ifndef RSKERNELSTEST_RSMATMUL_H
#define RSKERNELSTEST_RSMATMUL_H

#include "RScommon.h"


namespace androidrs {

namespace matmul {

// float
void rsMatmul_sgemm(void* a_ptr, bool a_trans, void* b_ptr, bool b_trans, void*& c_ptr,
                    int m, int n, int k, float alpha, float beta)
{
    if(!androidrs::mRS->getContext()){
        androidrs::mRS->init(androidrs::cachePath);
    }

    sp<const Element> e = Element::F32(androidrs::mRS);

    sp<const Type> a_t = Type::create(androidrs::mRS, e, k, m, 0);
    sp<const Type> b_t = Type::create(androidrs::mRS, e, n, k, 0);
    sp<const Type> c_t = Type::create(androidrs::mRS, e, n, m, 0);

    sp<Allocation> a_alloc = Allocation::createTyped(androidrs::mRS, a_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> b_alloc = Allocation::createTyped(androidrs::mRS, b_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> c_alloc = Allocation::createTyped(androidrs::mRS, c_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);

    a_alloc->copy2DRangeFrom(0, 0, k, m, a_ptr);
    b_alloc->copy2DRangeFrom(0, 0, n, k, b_ptr);

    sp<ScriptIntrinsicBLAS> sc = ScriptIntrinsicBLAS::create(androidrs::mRS);

    RsBlasTranspose a_transpose = a_trans ? RsBlasTranspose::RsBlasTrans : RsBlasTranspose::RsBlasNoTrans;
    RsBlasTranspose b_transpose = b_trans ? RsBlasTranspose::RsBlasTrans : RsBlasTranspose::RsBlasNoTrans;

    sc->SGEMM(a_transpose, b_transpose, alpha, a_alloc, b_alloc, beta, c_alloc);

    c_alloc->copy2DRangeTo(0, 0, n, m, c_ptr);
};

// uint8_t
void rsMatmul_bnnm(void* a_ptr, int a_off, void* b_ptr, int b_off, void*& c_ptr, int c_off,
                    int m, int n, int k, int c_mult)
{
    if(!androidrs::mRS->getContext()){
        androidrs::mRS->init(androidrs::cachePath);
    }
    sp<const Element> e = Element::U8(androidrs::mRS);

    sp<const Type> a_t = Type::create(androidrs::mRS, e, k, m, 0);
    sp<const Type> b_t = Type::create(androidrs::mRS, e, k, n, 0);
    sp<const Type> c_t = Type::create(androidrs::mRS, e, n, m, 0);

    sp<Allocation> a_alloc = Allocation::createTyped(androidrs::mRS, a_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> b_alloc = Allocation::createTyped(androidrs::mRS, b_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> c_alloc = Allocation::createTyped(androidrs::mRS, c_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);

    a_alloc->copy2DRangeFrom(0, 0, k, m, a_ptr);
    b_alloc->copy2DRangeFrom(0, 0, k, n, b_ptr);

    sp<ScriptIntrinsicBLAS> sc = ScriptIntrinsicBLAS::create(androidrs::mRS);

    sc->BNNM(a_alloc, a_off, b_alloc, b_off, c_alloc, c_off, c_mult);

    c_alloc->copy2DRangeTo(0, 0, n, m, c_ptr);
};

}
}

#endif //RSKERNELSTEST_RSMATMUL_H
