#ifndef _MY_NORM_H_
#define _MY_NORM_H_

#include <ap_int.h>
#include <hls_math.h>
#include <iostream>
#include <hls_stream.h>

using namespace std;

#define K 32
#define CH_DIV_K 16
#define HEIGHT_IN 38
#define WIDTH_IN 38

typedef ap_int<16>   dtype_dat;
typedef ap_int<16*K> dtype_bus;

typedef ap_int<32>   dtype_mul;
typedef ap_int<32*K> dtype_mul_bus;

typedef struct
{
	dtype_bus data;
	bool last;
}dtype_stream;

void Norm(ap_uint<16> ch_div_K,ap_uint<16> Hin,ap_uint<16> Win,
		hls::stream<dtype_bus> &feature_in,ap_uint<4> feature_in_precision,
		//dtype_dat W[],ap_uint<4> W_precision,
		//ap_int<32> norm[],
		dtype_mul_bus norm[],
		hls::stream<dtype_stream> &feature_out,ap_uint<4> feature_out_precision
		);

void Norm_1s(hls::stream<dtype_bus> &in,ap_uint<4> feature_in_precision,
		hls::stream<dtype_bus> &out,ap_uint<4> feature_out_precision,
		ap_uint<16> ch_div_K,ap_uint<16> Hin,ap_uint<16> Win,
		//ap_int<32> norm[]
		dtype_mul_bus norm[]);


void s1_hs2axis(hls::stream<dtype_bus> &in,
		hls::stream<dtype_stream> &out,
		ap_uint<16> ch_div_K,ap_uint<16> Hin,ap_uint<16> Win);


#endif
