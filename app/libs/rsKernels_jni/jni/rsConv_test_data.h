#include "common.h"
#include <string.h>

const int conv1_input_sz = 8128;
const int conv1_filter_sz = 65024;
const int conv1_output_sz = 2048;

const int conv3_input_sz = 200704;
const int conv3_filter_sz = 110592;
const int conv3_output_sz = 602112;

const int conv5_input_sz = 12544;
const int conv5_filter_sz = 12800;
const int conv5_output_sz = 25088;

const int conv7_input_sz = 150528;
const int conv7_filter_sz = 9408;
const int conv7_output_sz = 802816;


void* getConv1_input_F32();
void* getConv1_filter_F32();
void* getConv1_output_F32();

void* getConv3_input_F32();
void* getConv3_filter_F32();
void* getConv3_output_F32();

void* getConv5_input_F32();
void* getConv5_filter_F32();
void* getConv5_output_F32();

void* getConv7_input_F32();
void* getConv7_filter_F32();
void* getConv7_output_F32();