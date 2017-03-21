#pragma version(1)
#pragma rs java_package_name(com.foo.bar)
#pragma rs_fp_relaxed

int32_t in_depth;
int32_t input_rows;
int32_t input_cols;
int32_t filter_rows;
int32_t filter_cols;
int32_t stride_rows;
int32_t stride_cols;
int32_t pad_rows;
int32_t pad_cols;
int32_t out_depth;
int32_t out_rows;
int32_t out_cols;

int32_t filter_w;
int32_t filter_sz;
int32_t filter_stride_e;
int32_t input_stride_e;
int32_t padded_rows;
int32_t padded_cols;
int32_t filter_hf;

rs_allocation filters;
rs_allocation inputs;


void initParam()
{
    filter_w = filter_rows;
    filter_sz = filter_w * filter_w;
    filter_stride_e = out_depth * in_depth;
    input_stride_e = in_depth;
    padded_rows = input_rows + 2 * pad_rows;
    padded_cols = input_cols + 2 * pad_cols;
    filter_hf = filter_rows / 2;
}

float RS_KERNEL launchConvF32(uint32_t x)
{
    uint32_t row, col, lay;
    float sum = 0;
    uint32_t x_filter_off = 0, y_filter_off = 0;

    uint32_t out_z = x % out_depth;
    uint32_t out_y = (x / out_depth) % out_rows;
    uint32_t out_x = (x / out_depth) / out_rows;
    uint32_t in_centerX = out_x * stride_cols + filter_hf - pad_cols;
    uint32_t in_centerY = out_y * stride_rows + filter_hf - pad_rows;

    uint32_t x_lo = max((int32_t)in_centerX-filter_hf, 0);
    uint32_t x_hi = min((int32_t)in_centerX+filter_hf, input_cols-1);
    uint32_t y_lo = max((int32_t)in_centerY-filter_hf, 0);
    uint32_t y_hi = min((int32_t)in_centerY+filter_hf, input_rows-1);

    if((int32_t)in_centerX - filter_hf < 0){
        x_filter_off = filter_hf - (int32_t)in_centerX;
    }
    if((int32_t)in_centerY - filter_hf < 0){
        y_filter_off = filter_hf - (int32_t)in_centerY;
    }

    float input_ele, filter_ele;
    for(row=x_lo;row<=x_hi;++row){
        for(col=y_lo;col<=y_hi;++col){
            for(lay=0;lay<in_depth;++lay){
                input_ele = rsGetElementAt_float(inputs, lay + (row * input_rows + col) * in_depth);
                filter_ele = rsGetElementAt_float(filters, 
                                out_z + lay * out_depth + ((row - x_lo + x_filter_off) * filter_w + col - y_lo + y_filter_off) * filter_stride_e);
                sum += input_ele * filter_ele;
            }
        }
    }

    return sum;
}

uint8_t RS_KERNEL launchConvU8(uint32_t x, uint32_t y)
{
    uint32_t row, col, lay;
    uint8_t sum = 0;
    uint32_t x_filter_off = 0, y_filter_off = 0;

    uint32_t out_z = x % out_depth;
    uint32_t out_y = (x / out_depth) % out_rows;
    uint32_t out_x = (x / out_depth) / out_rows;
    uint32_t in_centerX = out_x * stride_cols + filter_hf - pad_cols;
    uint32_t in_centerY = out_y * stride_rows + filter_hf - pad_rows;

    uint32_t x_lo = max((int32_t)in_centerX-filter_hf, 0);
    uint32_t x_hi = min((int32_t)in_centerX+filter_hf, input_cols-1);
    uint32_t y_lo = max((int32_t)in_centerY-filter_hf, 0);
    uint32_t y_hi = min((int32_t)in_centerY+filter_hf, input_rows-1);

    if((int32_t)in_centerX - filter_hf < 0){
        x_filter_off = filter_hf - (int32_t)in_centerX;
    }
    if((int32_t)in_centerY - filter_hf < 0){
        y_filter_off = filter_hf - (int32_t)in_centerY;
    }

    uint8_t input_ele, filter_ele;
    for(row=x_lo;row<=x_hi;++row){
        for(col=y_lo;col<=y_hi;++col){
            for(lay=0;lay<in_depth;++lay){
                input_ele = rsGetElementAt_uchar(inputs, lay + (row * input_rows + col) * in_depth);
                filter_ele = rsGetElementAt_uchar(filters, 
                                out_z + lay * out_depth + ((row - x_lo + x_filter_off) * filter_w + col - y_lo + y_filter_off) * filter_stride_e);
                sum += input_ele * filter_ele;
            }
        }
    }

    return sum;
}