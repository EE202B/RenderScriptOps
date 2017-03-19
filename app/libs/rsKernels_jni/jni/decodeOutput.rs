#pragma version(1)
#pragma rs java_package_name(com.foo.bar)
#pragma rs_fp_relaxed


int32_t inputRows;
int32_t inputCols;
int32_t padRows;
int32_t padCols;
int32_t outDepth;
int32_t strideRows;
int32_t strideCols;
int32_t outCols;
int32_t startIdx;

float* allOutPtrF32;
rs_allocation layerInPtrF32;

float RS_KERNEL decode_F32(uint32_t x, uint32_t y)
{
    if((x>=padCols)&&(x<=inputCols+padCols-1)
        &&(y>=padRows)&&(y<=inputRows+padRows-1)
        &&((x-padCols)%strideCols==0)&&((y-padRows)%strideRows==0))
    {
        uint32_t idx = startIdx + (((x-padCols)/strideCols)*outCols + ((y-padRows)/strideRows))*outDepth;
        allOutPtrF32[idx] = rsGetElementAt_float(layerInPtrF32, x, y);
    }

    return 0;
}