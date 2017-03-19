#pragma version(1)
#pragma rs java_package_name(com.foo.bar)
#pragma rs_fp_relaxed

int32_t filterW;
int32_t decodeStride;
int32_t startIdx;
float* allPtrF32;

float RS_KERNEL decode_F32(uint32_t x)
{
    uint32_t idx = startIdx + ((x%filterW) * filterW + x/filterW) * decodeStride;
    return allPtrF32[idx];
}