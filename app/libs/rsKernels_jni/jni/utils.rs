#pragma version(1)
#pragma rs java_package_name(com.foo.bar)
#pragma rs_fp_relaxed

void root(const float* in, float* out)
{
    *out += *in;
}
