#include "rsConv.h"

#include "ScriptC_utils.h"

void dummyTest(const char * path)
{
    float kernel[] = {
            1, 2, 1,
            0, 0, 0,
            -1, -2, -1,
    };
    float input[] = {
            0, 0, 0, 0, 0,
            0, 1, 2, 3, 0,
            0, 4, 5, 6, 0,
            0, 7, 8, 9, 0,
            0, 0, 0, 0, 0,
    };
    float input2[] = {
            0, 0, 0, 0, 0,
            0, 100, 200, 300, 0,
            0, 400, 500, 600, 0,
            0, 700, 800, 900, 0,
            0, 0, 0, 0, 0,
    };
    size_t k = 5;

    sp<RS> rs = new RS();
    rs->init(path);
    sp<const Element> e = Element::F32(rs);

    sp<const Type> input_t = Type::create(rs, e, k, k, 0);
    sp<const Type> output_t = Type::create(rs, e, k, k, 0);

    sp<Allocation> input_alloc = Allocation::createTyped(rs, input_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> output_alloc = Allocation::createTyped(rs, output_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    input_alloc->copy2DRangeFrom(0, 0, k, k, static_cast<void*>(input));

    sp<Allocation> input2_alloc = Allocation::createTyped(rs, input_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> output2_alloc = Allocation::createTyped(rs, output_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    input2_alloc->copy2DRangeFrom(0, 0, k, k, static_cast<void*>(input2));

    sp<ScriptIntrinsicConvolve3x3> sc = ScriptIntrinsicConvolve3x3::create(rs, e);
    
    sc->setCoefficients(kernel);
    sc->setInput(input_alloc);
    sc->forEach(output_alloc);

    sc->setCoefficients(kernel);
    sc->setInput(input2_alloc);
    sc->forEach(output2_alloc);

    void* out = new float[k*k];
    output_alloc->copy2DRangeTo(0, 0, k, k, out);

    void* out2 = new float[k*k];
    output2_alloc->copy2DRangeTo(0, 0, k, k, out2);

    for(int i=0;i<k*k;++i){
        LOGI("%f", static_cast<float*>(out)[i]);
    }
    LOGE("Second rs");
    for(int i=0;i<k*k;++i){
        LOGI("%f", static_cast<float*>(out2)[i]);
    }

    delete[] static_cast<float*>(out);
    delete[] static_cast<float*>(out2);
}

void scriptTest(const char * path)
{
    float input[] = {
            0, 0, 0, 0, 0,
            0, 1, 2, 3, 0,
            0, 4, 5, 6, 0,
            0, 7, 8, 9, 0,
            0, 0, 0, 0, 0,
    };
    size_t k = 5;

    sp<RS> rs = new RS();
    rs->init(path);
    sp<const Element> e = Element::F32(rs);

    sp<const Type> data_t = Type::create(rs, e, k, k, 0);

    sp<Allocation> input_alloc = Allocation::createTyped(rs, data_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> output_alloc = Allocation::createTyped(rs, data_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);

    input_alloc->copy2DRangeFrom(0, 0, k, k, static_cast<void*>(input));

    ScriptC_utils* sc = new ScriptC_utils(rs);
    // sc->forEach_root(input_alloc, output_alloc);
    // rs->finish();
    // sc->forEach_root(input_alloc, output_alloc);
    // rs->finish();

    void* out = new float[k*k];
    output_alloc->copy2DRangeTo(0, 0, k, k, out);

    for(int i=0;i<k*k;++i){
        LOGI("%f", static_cast<float*>(out)[i]);
    }

    delete[] static_cast<float*>(out);
}

// use Intrinsic, memcpy, input must be padded, conv kernel stride is fixed at 1, the output padded area are garbage
// TODO: add ScriptGroup
void rsConv3_3_float_intrinsic(const char * path, void* filter, void* input, void*& output, rsConvInfo convInfo)
{   
    // scriptTest(path);
    // return;

    const size_t filter_w = 3;
    const size_t filter_sz = filter_w * filter_w;

    sp<RS> rs = new RS();
    rs->init(path);
    sp<const Element> e;
    size_t filter_stride_e = convInfo.out_depth * convInfo.in_depth;
    size_t input_stride_e = convInfo.in_depth;

    if(convInfo.data_format==0){
        e = Element::F32(rs);
    }else{
        return;
    }

    // decode filters
    size_t e_bytes = e->getSizeBytes();
    int filters_total_size = convInfo.filter_cols * convInfo.filter_rows * convInfo.in_depth * convInfo.out_depth;
    std::vector<std::vector<float* > > mFilters2D(
        convInfo.out_depth, std::vector<float* >(convInfo.in_depth, nullptr)
    );
    //TODO: use rs
    for(size_t i=0;i<convInfo.in_depth;++i){
        for(size_t j=0;j<convInfo.out_depth;++j){
            auto filter_transposed = static_cast<float*>(filter) + (i * convInfo.out_depth + j);
            mFilters2D[j][i] = new float[filter_sz];

            for(size_t p=0;p<filter_w;++p){
                for(size_t q=0;q<filter_w;++q){
                    mFilters2D[j][i][p * filter_w + q] = filter_transposed[(q * filter_w + p) * filter_stride_e];
                }
            }
        }
    }

    // for(int i=0;i<mFilters2D.size();++i){
    //     for(int j = 0;j<mFilters2D[i].size();++j){
    //         for(int k=0;k<9;++k){
    //             LOGI("%f", mFilters2D[i][j][k]);
    //         }
    //         LOGE("one sub filter");
    //     }
    //     LOGE("one filter");
    // }

    // decode input
    auto input_cast_float = static_cast<float*>(input);
    sp<const Type> input_layer_t = Type::create(rs, e, convInfo.input_cols,
                                                       convInfo.input_rows,
                                                       0);
    std::vector<sp<Allocation > > intput_layers;
    //TODO: use rs
    for(size_t k=0;k<convInfo.in_depth;++k){
        sp<Allocation > input_alloc = Allocation::createTyped(rs, input_layer_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
        size_t input_alloc_stride;
        auto input_alloc_ptr = static_cast<float*>(input_alloc->getPointer(&input_alloc_stride));
        input_alloc_stride /= sizeof(float);

        for (size_t i = 0; i < convInfo.input_rows; i++) {
            for (size_t j = 0; j < convInfo.input_cols; j++) {
                input_alloc_ptr[i * input_alloc_stride + j] = (input_cast_float + k)[(j * convInfo.input_cols + i) * input_stride_e];
            }
        }
        intput_layers.push_back(input_alloc);
    }

    // for(size_t k=0;k<intput_layers.size();++k){
    //     size_t input_alloc_stride;
    //     auto tmp = static_cast<float*>(intput_layers[k]->getPointer(&input_alloc_stride));
    //     input_alloc_stride /= sizeof(float);
    //     for (size_t i = 0; i < convInfo.input_rows; i++) {
    //         for (size_t j = 0; j < convInfo.input_cols; j++) {
    //             LOGI("%f", tmp[i * input_alloc_stride + j]);
    //         }
    //         LOGE("One row");
    //     }
    //     LOGE("One layer");
    // }

    // Conv
    sp<const Type> output_layer_t = Type::create(rs, e, convInfo.input_cols,
                                                        convInfo.input_rows,
                                                        0);
    std::vector<std::vector<sp<Allocation> > > output_filters_reponse(
        convInfo.out_depth, std::vector<sp<Allocation> >(convInfo.in_depth, NULL)
    );   
    for(size_t i=0;i<mFilters2D.size();++i){       
        for(size_t j=0;j<mFilters2D[i].size();++j){
            sp<Allocation > output_alloc_filter = Allocation::createTyped(rs, output_layer_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
            output_filters_reponse[i][j] = output_alloc_filter;

            sp<ScriptIntrinsicConvolve3x3> sc = ScriptIntrinsicConvolve3x3::create(rs, e);
            sc->setCoefficients(mFilters2D[i][j]);
            sc->setInput(intput_layers[j]);
            sc->forEach(output_alloc_filter);
        }
    }
    rs->finish();

    // for(int i=0;i<output_filters_reponse.size();++i){
    //     for(int j=0;j<output_filters_reponse[i].size();++j){
    //         size_t stride;
    //         auto tmp = static_cast<float*>(output_filters_reponse[i][j]->getPointer(&stride));
    //         stride /= sizeof(float);
    //         for(int p=0;p<convInfo.input_rows;++p){
    //             for(int q = 0;q<convInfo.input_cols;++q){
    //                 LOGI("%f", tmp[p * stride + q]);
    //             }
    //             LOGE("One row");
    //         }
    //         LOGE("One layer");
    //     }
    // }

    // sum up
    std::vector<sp<Allocation>> output_alloc_final;
    sp<ScriptC_utils> sc = new ScriptC_utils(rs);
    for(size_t i=0;i<output_filters_reponse.size();++i){
        sp<Allocation > output_alloc_filter = Allocation::createTyped(rs, output_layer_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
        output_alloc_final.push_back(output_alloc_filter);

        for(size_t j=0;j<output_filters_reponse[i].size();++j){
            sc->forEach_root(output_filters_reponse[i][j], output_alloc_filter);
            rs->finish();
        }
    }

    size_t output_row_sz = (convInfo.input_rows - 2*convInfo.pad_rows - 1) / convInfo.conv_stride + 1;

    for(int k=0;k<output_alloc_final.size();++k){
        size_t stride;
        auto tmp = static_cast<float*>(output_alloc_final[k]->getPointer(&stride));
        stride /= sizeof(float);

        for(int i=convInfo.pad_rows;i<convInfo.input_rows-convInfo.pad_cols;i+=convInfo.conv_stride){
            for(int j=convInfo.pad_cols;j<convInfo.input_cols-convInfo.pad_cols;j+=convInfo.conv_stride){
                size_t out_idx = (i-convInfo.pad_rows)*output_row_sz/convInfo.conv_stride + (j-convInfo.pad_cols)/convInfo.conv_stride;
                static_cast<float*>(output)[out_idx*output_alloc_final.size() + k] = tmp[j * stride + i];
            }
        }
    }

    for(int i=0;i<mFilters2D.size();++i){
        for(int j = 0;j<mFilters2D[i].size();++j){
            delete[] mFilters2D[i][j];
        }
    }
}

// Use custom script, no memcpy, input don't need padding, conv kernel stride is user defined, the output size smaller than input
// T(conv(T(A), T(B)))
void rsConv3_3_float_script(const char * path, void* filter, void* input, void*& output, rsConvInfo convInfo)
{
    sp<RS> rs = new RS();
    rs->init(path);
    sp<const Element> e;
    size_t filter_stride = convInfo.out_depth * convInfo.in_depth;
    size_t input_stride = convInfo.in_depth;

    const size_t filter_w = 3;
    const size_t filter_sz = filter_w * filter_w;

    if(convInfo.data_format==0){
        e = Element::F32(rs);
    }else{
        return;
    }

    size_t e_bytes = e->getSizeBytes();
    int filters_total_size = convInfo.filter_cols * convInfo.filter_rows * convInfo.in_depth * convInfo.out_depth;

    std::vector<std::vector<float* > > mFilters2D(
        convInfo.out_depth, std::vector<float* >(convInfo.in_depth, nullptr)
    );

    for(size_t i=0;i<convInfo.in_depth;++i){
        for(size_t j=0;j<convInfo.out_depth;++j){
            mFilters2D[j][i] = static_cast<float*>(filter) + (i * convInfo.out_depth + j);
        }
    }
    /*
    for(int i=0;i<mFilters2D.size();++i){
        for(int j = 0;j<mFilters2D[i].size();++j){
            for(int k=0;k<filter_sz;k++){
             LOGI("%f", mFilters2D[i][j][k*filter_stride]);
            }
        }
    }*/

}