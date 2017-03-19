//
// Created by WangYingnan on 3/12/17.
//

#ifndef RSKERNELSTEST_RSCONV_H
#define RSKERNELSTEST_RSCONV_H

#include "common.h"
#include <vector>
#include <memory>
#include "RenderScript.h"
#include "ScriptC_utils.h"

using namespace android::RSC;

namespace androidrs {

//TODO:
// static sp<RS> rs = new RS();

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
    int data_format; // 0 F32, 1 U8

    rsConvInfo(int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10, int n11, int n12, int n13, int n14){
        in_depth=n1;
        input_rows=n2+n8*2;
        input_cols=n3+n9*2;
        filter_rows=n4;filter_cols=n5;
        stride_rows=n6;stride_cols=n7;
        pad_rows=n8;pad_cols=n9;
        out_depth=n10;out_rows=n11;out_cols=n12;
        batch=n13;data_format=n14;
    };
};

// use Intrinsic, memcpy, input must be padded, conv kernel stride is fixed at 1, the output padded area are garbage
template <typename T>
void rsConv_intrinsic(const char * path, void* filter, void* input, void*& output, rsConvInfo convInfo)
{
    // assume square filter
    const size_t filter_w = convInfo.filter_rows;
    const size_t filter_sz = filter_w * filter_w;

    sp<RS> rs = new RS();
    rs->init(path);
    sp<const Element> e;
    size_t filter_stride_e = convInfo.out_depth * convInfo.in_depth;
    size_t input_stride_e = convInfo.in_depth;

    if(convInfo.data_format==0){
        e = Element::F32(rs);
    }else{
        e = Element::U8(rs);
    }
    size_t e_bytes = e->getSizeBytes();

    // decode filters
    std::vector<std::vector<float* > > mFilters2D(
        convInfo.out_depth, std::vector<float* >(convInfo.in_depth, nullptr)
    );
    //TODO: use rs
    for(size_t i=0;i<convInfo.in_depth;++i){
        for(size_t j=0;j<convInfo.out_depth;++j){
            auto filter_transposed = static_cast<T*>(filter) + (i * convInfo.out_depth + j);
            mFilters2D[j][i] = new float[filter_sz];

            for(size_t p=0;p<filter_w;++p){
                for(size_t q=0;q<filter_w;++q){
                    mFilters2D[j][i][p * filter_w + q] = (float)filter_transposed[(q * filter_w + p) * filter_stride_e];
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
    auto input_cast = static_cast<T*>(input);
    sp<const Type> input_layer_t = Type::create(rs, e, convInfo.input_cols,
                                                       convInfo.input_rows,
                                                       0);
    std::vector<sp<Allocation > > intput_layers;
    //TODO: use rs
    for(size_t k=0;k<convInfo.in_depth;++k){
        sp<Allocation > input_alloc = Allocation::createTyped(rs, input_layer_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
        size_t input_alloc_stride;
        auto input_alloc_ptr = static_cast<T*>(input_alloc->getPointer(&input_alloc_stride));
        input_alloc_stride /= e_bytes;

        for (size_t i = 0; i < convInfo.input_rows; i++) {
            for (size_t j = 0; j < convInfo.input_cols; j++) {
                input_alloc_ptr[i * input_alloc_stride + j] = (input_cast + k)[(j * convInfo.input_cols + i) * input_stride_e];
            }
        }
        intput_layers.push_back(input_alloc);
    }

    // for(size_t k=0;k<intput_layers.size();++k){
    //     size_t input_alloc_stride;
    //     auto tmp = static_cast<T*>(intput_layers[k]->getPointer(&input_alloc_stride));
    //     input_alloc_stride /= e_bytes;
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
    //         auto tmp = static_cast<T*>(output_filters_reponse[i][j]->getPointer(&stride));
    //         stride /= e_bytes;
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
    if(convInfo.data_format==0){
        for(size_t i=0;i<output_filters_reponse.size();++i){
            sp<Allocation > output_alloc_filter = Allocation::createTyped(rs, output_layer_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
            output_alloc_final.push_back(output_alloc_filter);
            for(size_t j=0;j<output_filters_reponse[i].size();++j){
                sc->forEach_sumAlloc_F32(output_filters_reponse[i][j], output_alloc_filter);
                rs->finish();
            }
        }
    }else{
        for(size_t i=0;i<output_filters_reponse.size();++i){
            sp<Allocation > output_alloc_filter = Allocation::createTyped(rs, output_layer_t, RS_ALLOCATION_USAGE_SHARED | RS_ALLOCATION_USAGE_SCRIPT);
            output_alloc_final.push_back(output_alloc_filter);
            for(size_t j=0;j<output_filters_reponse[i].size();++j){
                sc->forEach_sumAlloc_U8(output_filters_reponse[i][j], output_alloc_filter);
                rs->finish();
            }
        }
    }

    // size_t output_row_sz = (convInfo.input_rows - 2*convInfo.pad_rows - 1) / convInfo.stride_rows + 1;
    // size_t output_col_sz = (convInfo.input_cols - 2*convInfo.pad_cols - 1) / convInfo.stride_cols + 1;

    for(int k=0;k<output_alloc_final.size();++k){
        size_t stride;
        auto tmp = static_cast<T*>(output_alloc_final[k]->getPointer(&stride));
        stride /= e_bytes;

        for(int i=convInfo.pad_rows;i<convInfo.input_rows-convInfo.pad_cols;i+=convInfo.stride_rows){
            for(int j=convInfo.pad_cols;j<convInfo.input_cols-convInfo.pad_cols;j+=convInfo.stride_cols){
                size_t out_idx = ((j-convInfo.pad_cols)/convInfo.stride_cols)*convInfo.out_cols + ((i-convInfo.pad_rows)/convInfo.stride_rows);
                static_cast<T*>(output)[out_idx * output_alloc_final.size() + k] = tmp[i * stride + j];
            }
        }
    }

    for(int i=0;i<mFilters2D.size();++i){
        for(int j = 0;j<mFilters2D[i].size();++j){
            delete[] mFilters2D[i][j];
        }
    }
};

template <typename T>
void rsConv5_intrinsic(const char * path, void* filter, void* input, void*& output, rsConvInfo convInfo){

};

// Use custom script, no memcpy, input don't need padding, conv kernel stride is user defined, the output size smaller than input
// T(conv(T(A), T(B)))

template <typename T>
void rsConv1_script(const char * path, void* filter, void* input, void*& output, rsConvInfo convInfo){
    
};

template <typename T>
void rsConv3_script(const char * path, void* filter, void* input, void*& output, rsConvInfo convInfo)
{
    // sp<RS> rs = new RS();
    // rs->init(path);
    // sp<const Element> e;
    // size_t filter_stride = convInfo.out_depth * convInfo.in_depth;
    // size_t input_stride = convInfo.in_depth;

    // const size_t filter_w = 3;
    // const size_t filter_sz = filter_w * filter_w;

    // if(convInfo.data_format==0){
    //     e = Element::F32(rs);
    // }else{
    //     return;
    // }

    // size_t e_bytes = e->getSizeBytes();
    // int filters_total_size = convInfo.filter_cols * convInfo.filter_rows * convInfo.in_depth * convInfo.out_depth;

    // std::vector<std::vector<float* > > mFilters2D(
    //     convInfo.out_depth, std::vector<float* >(convInfo.in_depth, nullptr)
    // );

    // for(size_t i=0;i<convInfo.in_depth;++i){
    //     for(size_t j=0;j<convInfo.out_depth;++j){
    //         mFilters2D[j][i] = static_cast<float*>(filter) + (i * convInfo.out_depth + j);
    //     }
    // }
    // /*
    // for(int i=0;i<mFilters2D.size();++i){
    //     for(int j = 0;j<mFilters2D[i].size();++j){
    //         for(int k=0;k<filter_sz;k++){
    //          LOGI("%f", mFilters2D[i][j][k*filter_stride]);
    //         }
    //     }
    // }*/

};

template <typename T>
void rsConv5_script(const char * path, void* filter, void* input, void*& output, rsConvInfo convInfo){
    
};

template <typename T>
void rsConv7_script(const char * path, void* filter, void* input, void*& output, rsConvInfo convInfo){
    
};

}
}


#endif //RSKERNELSTEST_RSCONV_H