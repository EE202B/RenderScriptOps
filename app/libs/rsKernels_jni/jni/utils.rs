#pragma version(1)
#pragma rs java_package_name(com.foo.bar)
#pragma rs_fp_relaxed

void sumAlloc_F32(const float* in, float* out)
{
    *out += *in;
}

void sumAlloc_U8(const uint8_t* in, uint8_t* out)
{
    *out += *in;
}