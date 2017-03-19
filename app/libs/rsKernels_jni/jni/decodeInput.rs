#pragma version(1)
#pragma rs java_package_name(com.foo.bar)
#pragma rs_fp_relaxed

int32_t inputRows;
int32_t inputCols;
int32_t padRows;
int32_t padCols;
int32_t decodeStride;
int32_t startIdx;
float* allPtrF32;

float RS_KERNEL decode_F32(uint32_t x, uint32_t y)
{
    if(x<padCols || x>(inputCols+padCols-1) 
        || y<padRows || y>(inputRows+padRows-1)){
        return 0;
    }

    uint32_t idx = startIdx + ((x-padCols)*inputCols+y-padRows)*decodeStride;
    return allPtrF32[idx];
}