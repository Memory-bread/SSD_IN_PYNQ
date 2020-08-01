#include "norm.h"

void Norm(ap_uint<16> ch_div_K,ap_uint<16> Hin,ap_uint<16> Win,
		hls::stream<dtype_bus> &feature_in,ap_uint<4> feature_in_precision,
		//dtype_dat W[],ap_uint<4> W_precision,
		//ap_int<32> norm[],
		dtype_mul_bus norm[],
		hls::stream<dtype_stream> &feature_out,ap_uint<4> feature_out_precision
		)

{
#pragma HLS INTERFACE m_axi depth=128 port=norm offset=slave
#pragma HLS INTERFACE axis register both port=feature_out
#pragma HLS INTERFACE axis register both port=feature_in
#pragma HLS INTERFACE s_axilite port=Win
#pragma HLS INTERFACE s_axilite port=feature_in_precision
#pragma HLS INTERFACE s_axilite port=Hin
#pragma HLS INTERFACE s_axilite port=feature_out_precision
#pragma HLS INTERFACE s_axilite port=ch_div_K
#pragma HLS INTERFACE s_axilite port=return


	dtype_bus out_tp16;
	hls::stream<dtype_bus> stream_tp;
	#pragma HLS STREAM variable=stream_tp depth=8 dim=1

	#pragma HLS DATAFLOW
	Norm_1s(feature_in,feature_in_precision,
			stream_tp,feature_out_precision,
			ch_div_K,Hin,Win,
			norm);

	s1_hs2axis(stream_tp,feature_out,ch_div_K,Hin,Win);

}



void Norm_1s(hls::stream<dtype_bus> &in,ap_uint<4> feature_in_precision,
		hls::stream<dtype_bus> &out,ap_uint<4> feature_out_precision,
		ap_uint<16> ch_div_K,ap_uint<16> Hin,ap_uint<16> Win,
		//ap_int<32> norm[]
		dtype_mul_bus norm[])
{
#pragma HLS INTERFACE ap_stable port=Hin
#pragma HLS INTERFACE ap_stable port=Win
#pragma HLS INTERFACE ap_stable port=ch_div_K
#pragma HLS INTERFACE ap_stable port=feature_in_precision
#pragma HLS INTERFACE ap_stable port=feature_out_precision

	std::cout<<"norm["<<ch_div_K<<"]"<<std::endl;

	for(int cin=0;cin<ch_div_K;cin++){
		for(int k=0;k<K;k++)
		{
			norm[cin].range(32*k+31,32*k)=0;
		}
	}

	//std::cout<<"norm["<<(int)ch_div_K<<"]"<<std::endl;
	ap_int<32> sum[512];
	for(int i=0;i<512;i++){
		sum[i]=0;
	}

	//std::cout<<"sum[3]="<<sum[3]<<std::endl;

	LOOP_cin:for(int cin=0;cin<ch_div_K;cin++)
	{
	#pragma HLS LOOP_TRIPCOUNT min=128 max=128 avg=128
		LOOP_i:for(int i=0;i<Hin;i++)
		{
			#pragma HLS LOOP_TRIPCOUNT min=38 max=38 avg=38
			LOOP_j:for(int j=0;j<Win;j++)
			{
				#pragma HLS LOOP_TRIPCOUNT min=38 max=38 avg=38
				#pragma HLS PIPELINE II=1
				//[CHin/K][H][W][K]
				dtype_bus in_block=in.read();
				dtype_bus tp_out16;
				for(int k=0;k<K;k++)
				{
					//std::cout<<"in_block["<<i<<"]["<<j<<"]["<<k<<"]="<<in_block.range(k*16+15,k*16)<<std::endl;
					tp_out16.range(k*16+15,k*16)=(ap_int<32>)((dtype_dat)in_block.range(k*16+15,k*16)*(dtype_dat)in_block.range(k*16+15,k*16))>>feature_in_precision;
					//std::cout<<"tp_out16["<<i<<"]["<<j<<"]["<<k<<"]="<<tp_out16.range(k*16+15,k*16)<<std::endl;
					sum[cin*K+k]+=tp_out16.range(k*16+15,k*16);
					if(i==Hin-1 && j==Win-1){
						//std::cout<<"sum["<<cin*K+k<<"]="<<sum[cin*K+k]<<std::endl;
						norm[cin].range(32*k+31,32*k)=sqrt_fixed(sum[cin*K+k]);
						//std::cout<<"norm["<<cin*K+k<<"]="<<norm[cin].range(32*k+31,32*k)<<std::endl;
					}
				}
				out.write(in_block);
			}
		}
	}
}




void s1_hs2axis(hls::stream<dtype_bus> &in,hls::stream<dtype_stream> &out,ap_uint<16> ch_div_K,ap_uint<16> Hin,ap_uint<16> Win)
{
	#pragma HLS INTERFACE ap_stable port=Hin
	#pragma HLS INTERFACE ap_stable port=Win
	#pragma HLS INTERFACE ap_stable port=ch_div_K

	for(int i=0;i<Hin*Win*ch_div_K;i++)
	{
		#pragma HLS PIPELINE II=1
		#pragma HLS LOOP_TRIPCOUNT min=200 max=200 avg=200

		dtype_stream tp;
		tp.data=in.read();
		if(i==(Hin*Win*ch_div_K-1))
			tp.last=1;
		else
			tp.last=0;
		out.write(tp);
	}
}








