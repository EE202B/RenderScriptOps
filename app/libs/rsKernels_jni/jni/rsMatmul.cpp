//
// Created by WangYingnan on 3/12/17.
//

#include "rsMatmul.h"

// single float
void rsMatmul_sgemm(const char * path, void* a_ptr, bool a_trans, void* b_ptr, bool b_trans, void*& c_ptr,
                    int m, int n, int k, float alpha, float beta)
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

    RsBlasTranspose a_transpose = a_trans ? RsBlasTranspose::RsBlasTrans : RsBlasTranspose::RsBlasNoTrans;
    RsBlasTranspose b_transpose = b_trans ? RsBlasTranspose::RsBlasTrans : RsBlasTranspose::RsBlasNoTrans;

    sc->SGEMM(a_transpose, b_transpose, alpha, a_alloc, b_alloc, beta, c_alloc);

    c_alloc->copy2DRangeTo(0, 0, n, m, c_ptr);
}

// single uint8_t
void rsMatmul_bnnm(const char * path, void* a_ptr, int a_off, void* b_ptr, int b_off, void*& c_ptr, int c_off,
                    int m, int n, int k, int c_mult)
{
    sp<RS> rs = new RS();
    rs->init(path);
    //hardcoded uint8_t
    sp<const Element> e = Element::U8(rs);

    sp<const Type> a_t = Type::create(rs, e, k, m, 0);
    sp<const Type> b_t = Type::create(rs, e, n, k, 0);
    sp<const Type> c_t = Type::create(rs, e, n, m, 0);

    sp<Allocation> a_alloc = Allocation::createTyped(rs, a_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> b_alloc = Allocation::createTyped(rs, b_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> c_alloc = Allocation::createTyped(rs, c_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);

    a_alloc->copy2DRangeFrom(0, 0, k, m, a_ptr);
    b_alloc->copy2DRangeFrom(0, 0, n, k, b_ptr);

    sp<ScriptIntrinsicBLAS> sc = ScriptIntrinsicBLAS::create(rs);

    sc->BNNM(a_alloc, a_off, b_alloc, b_off, c_alloc, c_off, c_mult);

    c_alloc->copy2DRangeTo(0, 0, n, m, c_ptr);
}