//
// Created by WangYingnan on 3/12/17.
//

#ifndef RSKERNELSTEST_RSCONV_H
#define RSKERNELSTEST_RSCONV_H

#include "RScommon.h"
#include <vector>
#include <memory>
#include "ScriptC_utils.h"
#include "ScriptC_decodeFilter.h"
#include "ScriptC_decodeInput.h"
#include "ScriptC_decodeOutput.h"
#include "ScriptC_mScriptConv.h"


namespace androidrs {

namespace conv {

struct rsConvInfo{
    int in_depth;
    int input_rows;
    int input_cols;

    int filter_rows;
    int filter_cols;

    int stride_rows;
    int stride_cols;

    int pad_rows;
    int pad_cols;

    int out_depth;
    int out_rows;
    int out_cols;

    int batch;
    int data_format; // 4 F32, 1 U8

    rsConvInfo(int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10, int n11, int n12, int n13, int n14){
        in_depth=n1;
        input_rows=n2;
        input_cols=n3;
        filter_rows=n4;filter_cols=n5;
        stride_rows=n6;stride_cols=n7;
        pad_rows=n8;pad_cols=n9;
        out_depth=n10;out_rows=n11;out_cols=n12;
        batch=n13;data_format=n14;
    };
};

// use Intrinsic, memcpy, input must be padded, conv kernel stride is fixed at 1, the output padded area are garbage
// Only support 3x3 and 5x5
// TODO: U8 mode 
// Deprecated!
template <typename T>
void rsConv_intrinsic(const char * path, void* filter, void* input, void*& output, rsConvInfo convInfo)
{
    // assume square filter
    const size_t filter_w = convInfo.filter_rows;
    const size_t filter_sz = filter_w * filter_w;
    const size_t filter_stride_e = convInfo.out_depth * convInfo.in_depth;
    const size_t input_stride_e = convInfo.in_depth;
    const size_t padded_rows = convInfo.input_rows + 2*convInfo.pad_rows;
    const size_t padded_cols = convInfo.input_cols + 2*convInfo.pad_cols;

    if(!androidrs::mRS->getContext()){
        androidrs::mRS->init(androidrs::cachePath);
    }
    sp<const Element> e, ef;

    ef = Element::F32(androidrs::mRS);
    if(convInfo.data_format==4){
        e = Element::F32(androidrs::mRS);
    }else if(convInfo.data_format==1){
        e = Element::U8(androidrs::mRS);
    }
    size_t e_bytes = e->getSizeBytes();

    // decode filters
    sp<const Type> all_filters_t = Type::create(androidrs::mRS, e, filter_stride_e*filter_sz,
                                                       0,
                                                       0);
    sp<Allocation > allFilters_alloc = Allocation::createTyped(androidrs::mRS, all_filters_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    allFilters_alloc->copy1DFrom(filter);
    
    sp<const Type> one_filter_t = Type::create(androidrs::mRS, ef, filter_sz,
                                                      0,
                                                      0);
    // std::vector<std::vector<float* > > mFilters2D(
    //     convInfo.out_depth, std::vector<float* >(convInfo.in_depth, nullptr)
    // );
    // TODO: U8 mode 
    std::vector<sp<Allocation>> mFilters2D;
    for(size_t i=0;i<convInfo.in_depth;++i){
        for(size_t j=0;j<convInfo.out_depth;++j){
            sp<Allocation> one_filter = Allocation::createTyped(androidrs::mRS, one_filter_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
            mFilters2D.push_back(one_filter);

            sp<ScriptC_decodeFilter> sc = new ScriptC_decodeFilter(androidrs::mRS);
            sc->set_filterW(filter_w);
            sc->set_decodeStride(filter_stride_e);
            sc->set_startIdx(i * convInfo.out_depth + j);
            sc->bind_allPtrF32(allFilters_alloc);
            sc->forEach_decode_F32(one_filter);

            // auto filter_transposed = static_cast<T*>(filter) + (i * convInfo.out_depth + j);
            // mFilters2D[j][i] = new float[filter_sz];
            // auto filter_transposed = static_cast<T*>(filter) + (i * convInfo.out_depth + j);
            // mFilters2D[j][i] = new float[filter_sz];
            // for(size_t p=0;p<filter_w;++p){
            //     for(size_t q=0;q<filter_w;++q){
            //         mFilters2D[j][i][p * filter_w + q] = (float)filter_transposed[(q * filter_w + p) * filter_stride_e];
            //     }
            // }
        }
    }
    androidrs::mRS->finish();

    // for(int i=0;i<convInfo.out_depth;++i){
    //     for(int j = 0;j<convInfo.in_depth;++j){
    //         for(int k=0;k<9;++k){
    //             LOGI("%f", static_cast<float*>(mFilters2D[j*convInfo.out_depth+i]->getPointer())[k]);
    //         }
    //         LOGE("one sub filter"); 
    //     }
    //     LOGE("one filter");
    // }
    // return;

    // decode input
    // auto input_cast = static_cast<T*>(input);
    sp<const Type> all_inputs_t = Type::create(androidrs::mRS, e, convInfo.in_depth*convInfo.input_rows*convInfo.input_cols,
                                                       0,
                                                       0);
    sp<Allocation > allInputs_alloc = Allocation::createTyped(androidrs::mRS, all_inputs_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    allInputs_alloc->copy1DFrom(input);

    sp<const Type> input_layer_t = Type::create(androidrs::mRS, e, padded_cols,
                                                       padded_rows,
                                                       0);
    std::vector<sp<Allocation > > intput_layers;

    for(size_t k=0;k<convInfo.in_depth;++k){
        sp<Allocation > input_alloc = Allocation::createTyped(androidrs::mRS, input_layer_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
        intput_layers.push_back(input_alloc);

        sp<ScriptC_decodeInput> sc = new ScriptC_decodeInput(androidrs::mRS);
        sc->set_inputRows(convInfo.input_rows);
        sc->set_inputCols(convInfo.input_cols);
        sc->set_padRows(convInfo.pad_rows);
        sc->set_padCols(convInfo.pad_cols);
        sc->set_decodeStride(input_stride_e);
        sc->set_startIdx(k);
        sc->bind_allPtrF32(allInputs_alloc);
        sc->forEach_decode_F32(input_alloc);

        // size_t input_alloc_stride;
        // auto input_alloc_ptr = static_cast<T*>(input_alloc->getPointer(&input_alloc_stride));
        // input_alloc_stride /= e_bytes;

        // for (size_t i = 0; i < convInfo.input_rows; i++) {
        //     for (size_t j = 0; j < convInfo.input_cols; j++) {
        //         input_alloc_ptr[(i + convInfo.pad_rows) * input_alloc_stride + j + convInfo.pad_cols]  
        //                 = (input_cast + k)[(j * convInfo.input_cols + i) * input_stride_e];      
        //     }
        // }
    }
    androidrs::mRS->finish();

    // for(size_t k=0;k<intput_layers.size();++k){
    //     size_t input_alloc_stride;
    //     auto tmp = static_cast<T*>(intput_layers[k]->getPointer(&input_alloc_stride));
    //     input_alloc_stride /= e_bytes;
    //     for (size_t i = 0; i < padded_rows; i++) {
    //         for (size_t j = 0; j < padded_cols; j++) {
    //             LOGI("%f", tmp[i * input_alloc_stride + j]);
    //         }
    //         LOGE("One row");
    //     }
    //     LOGE("One layer");
    // }
    // return;

    // Conv
    sp<const Type> output_layer_t = Type::create(androidrs::mRS, e, padded_cols,
                                                        padded_rows,
                                                        0);
    std::vector<std::vector<sp<Allocation> > > output_filters_reponse(
        convInfo.out_depth, std::vector<sp<Allocation> >(convInfo.in_depth, NULL)
    );   
    if(filter_w==3){
        for(size_t i=0;i<convInfo.out_depth;++i){       
            for(size_t j=0;j<convInfo.in_depth;++j){
                sp<Allocation > output_alloc_filter = Allocation::createTyped(androidrs::mRS, output_layer_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
                output_filters_reponse[i][j] = output_alloc_filter;

                sp<ScriptIntrinsicConvolve3x3> sc = ScriptIntrinsicConvolve3x3::create(androidrs::mRS, e);
                sc->setCoefficients(
                    static_cast<float*>(mFilters2D[j*convInfo.out_depth+i]->getPointer())
                );
                sc->setInput(intput_layers[j]);
                sc->forEach(output_alloc_filter);
            }
        }
    }else if(filter_w==5){
        for(size_t i=0;i<convInfo.out_depth;++i){       
            for(size_t j=0;j<convInfo.in_depth;++j){
                sp<Allocation > output_alloc_filter = Allocation::createTyped(androidrs::mRS, output_layer_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
                output_filters_reponse[i][j] = output_alloc_filter;

                sp<ScriptIntrinsicConvolve5x5> sc = ScriptIntrinsicConvolve5x5::create(androidrs::mRS, e);
                sc->setCoefficients(
                    static_cast<float*>(mFilters2D[j*convInfo.out_depth+i]->getPointer())
                );
                sc->setInput(intput_layers[j]);
                sc->forEach(output_alloc_filter);
            }
        }
    }
    androidrs::mRS->finish();

    // for(int i=0;i<output_filters_reponse.size();++i){
    //     for(int j=0;j<output_filters_reponse[i].size();++j){
    //         size_t stride;
    //         auto tmp = static_cast<T*>(output_filters_reponse[i][j]->getPointer(&stride));
    //         stride /= e_bytes;
    //         for(int p=0;p<convInfo.padded_rows;++p){
    //             for(int q = 0;q<padded_cols;++q){
    //                 LOGI("%f", tmp[p * stride + q]);
    //             }
    //             LOGE("One row");
    //         }
    //         LOGE("One layer");
    //     }
    // }

    // sum up
    std::vector<sp<Allocation>> output_alloc_final;
    sp<ScriptC_utils> sc = new ScriptC_utils(androidrs::mRS);
    if(convInfo.data_format==4){
        for(size_t i=0;i<output_filters_reponse.size();++i){
            sp<Allocation > output_alloc_filter = Allocation::createTyped(androidrs::mRS, output_layer_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
            output_alloc_final.push_back(output_alloc_filter);
            for(size_t j=0;j<output_filters_reponse[i].size();++j){
                sc->forEach_sumAlloc_F32(output_filters_reponse[i][j], output_alloc_filter);
                androidrs::mRS->finish();
            }
        }
    }else if(convInfo.data_format==1){
        for(size_t i=0;i<output_filters_reponse.size();++i){
            sp<Allocation > output_alloc_filter = Allocation::createTyped(androidrs::mRS, output_layer_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
            output_alloc_final.push_back(output_alloc_filter);
            for(size_t j=0;j<output_filters_reponse[i].size();++j){
                sc->forEach_sumAlloc_U8(output_filters_reponse[i][j], output_alloc_filter);
                androidrs::mRS->finish();
            }
        }
    }

    //output
    sp<const Type> all_outputs_t = Type::create(androidrs::mRS, e, convInfo.out_depth*convInfo.out_rows*convInfo.out_cols,
                                                       0,
                                                       0);
    sp<Allocation > allOutputs_alloc = Allocation::createTyped(androidrs::mRS, all_outputs_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);

    for(int k=0;k<output_alloc_final.size();++k){

        sp<ScriptC_decodeOutput> sc = new ScriptC_decodeOutput(androidrs::mRS);
        sc->set_inputRows(convInfo.input_rows);
        sc->set_inputCols(convInfo.input_cols);
        sc->set_padRows(convInfo.pad_rows);
        sc->set_padCols(convInfo.pad_cols);
        sc->set_outDepth(convInfo.out_depth);
        sc->set_strideRows(convInfo.stride_rows);
        sc->set_strideCols(convInfo.stride_cols);
        sc->set_outCols(convInfo.out_cols);
        sc->set_startIdx(k);

        sc->set_layerInPtrF32(output_alloc_final[k]);
        sc->bind_allOutPtrF32(allOutputs_alloc);
        sc->forEach_decode_F32(output_alloc_final[k]);

        // size_t stride;
        // auto tmp = static_cast<T*>(output_alloc_final[k]->getPointer(&stride));
        // stride /= e_bytes;
        // for(int i=convInfo.pad_rows;i<padded_rows-convInfo.pad_rows;i+=convInfo.stride_rows){
        //     for(int j=convInfo.pad_cols;j<padded_cols-convInfo.pad_cols;j+=convInfo.stride_cols){
        //         size_t out_idx = ((j-convInfo.pad_cols)/convInfo.stride_cols)*convInfo.out_cols + ((i-convInfo.pad_rows)/convInfo.stride_rows);
        //         static_cast<T*>(output)[out_idx * output_alloc_final.size() + k] = tmp[i * stride + j];
        //     }
        // }
    }
    allOutputs_alloc->copy1DTo(output);

    // for(int i=0;i<mFilters2D.size();++i){
    //     for(int j = 0;j<mFilters2D[i].size();++j){
    //         delete[] mFilters2D[i][j];
    //     }
    // }
};

// Use custom script, no memcpy, input don't need padding, conv kernel stride is user defined, the output size smaller than input
// T(conv(T(A), T(B)))
// pad_col must = filter_col / 2
template <typename T>
void rsConv_script(const char * path, void* filter, void* input, void*& output, rsConvInfo convInfo)
{
    const size_t filter_w = convInfo.filter_rows;
    const size_t filter_sz = filter_w * filter_w;
    const size_t filter_stride_e = convInfo.out_depth * convInfo.in_depth;
    const size_t input_stride_e = convInfo.in_depth;
    const size_t padded_rows = convInfo.input_rows + 2*convInfo.pad_rows;
    const size_t padded_cols = convInfo.input_cols + 2*convInfo.pad_cols;

    if(!androidrs::mRS->getContext()){
        androidrs::mRS->init(androidrs::cachePath);
    }
    sp<const Element> e;
    if(convInfo.data_format==4){
        e = Element::F32(androidrs::mRS);
    }else if(convInfo.data_format==1){
        e = Element::U8(androidrs::mRS);
    }
    size_t e_bytes = e->getSizeBytes();

    // alloc filter
    sp<const Type> all_filters_t = Type::create(androidrs::mRS, e, filter_stride_e*filter_sz,
                                                       0,
                                                       0);
    sp<Allocation > allFilters_alloc = Allocation::createTyped(androidrs::mRS, all_filters_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    allFilters_alloc->copy1DFrom(filter);

    // alloc input
    sp<const Type> all_inputs_t = Type::create(androidrs::mRS, e, convInfo.in_depth*convInfo.input_rows*convInfo.input_cols,
                                                       0,
                                                       0);
    sp<Allocation > allInputs_alloc = Allocation::createTyped(androidrs::mRS, all_inputs_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
    allInputs_alloc->copy1DFrom(input);

    //alloc output
    sp<const Type> all_outputs_t = Type::create(androidrs::mRS, e, convInfo.out_depth*convInfo.out_rows*convInfo.out_cols,
                                                       0,
                                                       0);
    sp<Allocation > allOutputs_alloc = Allocation::createTyped(androidrs::mRS, all_outputs_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);

    //kernel
    sp<ScriptC_mScriptConv> sc = new ScriptC_mScriptConv(androidrs::mRS);
    sc->set_in_depth(convInfo.in_depth);
    sc->set_input_rows(convInfo.input_rows);
    sc->set_input_cols(convInfo.input_cols);
    sc->set_filter_rows(convInfo.filter_rows);
    sc->set_filter_cols(convInfo.filter_cols);
    sc->set_stride_rows(convInfo.stride_rows);
    sc->set_stride_cols(convInfo.stride_cols);
    sc->set_pad_rows(convInfo.pad_rows);
    sc->set_pad_cols(convInfo.pad_cols);
    sc->set_out_depth(convInfo.out_depth);
    sc->set_out_rows(convInfo.out_rows);
    sc->set_out_cols(convInfo.out_cols);

    sc->set_filters(allFilters_alloc);
    sc->set_inputs(allInputs_alloc);
    sc->invoke_initParam();

    if(convInfo.data_format==4){
        sc->forEach_launchConvF32(allOutputs_alloc);
    }else if(convInfo.data_format==1){
        sc->forEach_launchConvU8(allOutputs_alloc);
    }

    // sync
    allOutputs_alloc->copy1DTo(output);
};

}
}


#endif //RSKERNELSTEST_RSCONV_H