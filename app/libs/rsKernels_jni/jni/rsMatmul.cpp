//
// Created by WangYingnan on 3/12/17.
//

#include "rsMatmul.h"

// single float
void rsMatmul_sgemm(const char * path, void* a_ptr, void* b_ptr, void*& c_ptr, int m, int n, int k)
{
    sp<RS> rs = new RS();
    rs->init(path);
    // hardcoded single float precision
    sp<const Element> e = Element::F32(rs);

    sp<const Type> a_t = Type::create(rs, e, k, m, 0);
    sp<const Type> b_t = Type::create(rs, e, n, k, 0);
    sp<const Type> c_t = Type::create(rs, e, n, m, 0);

    sp<Allocation> a_alloc = Allocation::createTyped(rs, a_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> b_alloc = Allocation::createTyped(rs, b_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> c_alloc = Allocation::createTyped(rs, c_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);

    a_alloc->copy2DRangeFrom(0, 0, k, m, a_ptr);
    b_alloc->copy2DRangeFrom(0, 0, n, k, b_ptr);

    sp<ScriptIntrinsicBLAS> sc = ScriptIntrinsicBLAS::create(rs);

    // hardcoded single float and no transpose
    sc->SGEMM(RsBlasTranspose::RsBlasNoTrans, RsBlasTranspose::RsBlasNoTrans, 1.0f, a_alloc, b_alloc, 0.0f, c_alloc);

    c_ptr = new DATA_TYPE[m*n];
    c_alloc->copy2DRangeTo(0, 0, n, m, c_ptr);
}
