#include "stdio.h"
#include "norm.h"
#include <ap_int.h>



int main(void)
{
	hls::stream<dtype_bus> in;
	hls::stream<dtype_stream> out;
	ap_uint<16> ch_div_K=CH_DIV_K;
	ap_uint<16> height_in=HEIGHT_IN;
	ap_uint<16> width_in=WIDTH_IN;

	dtype_mul_bus norm_out[CH_DIV_K];

	for(int c=0;c<ch_div_K;c++)
	{
		for(int i=0;i<height_in;i++)
		{
			for(int j=0;j<width_in;j++)
			{
				dtype_bus tp;
				for(int k=0;k<K;k++)
					if(i%2==0){
						tp.range(16*k+15,16*k)=14745;//(1<<14);//i*IN_WIDTH+j; =value*2^feature_in_precision
					}
					else{
						tp.range(16*k+15,16*k)=8192;
					}
						//std::cout<<"OUT["<<cin<<"]["<<i<<"]["<<j<<"]="<<(dtype_dat)feature_in[cin][i][j].range(16*k+15,16*k)<<std::endl;
				in.write(tp);
			}

		}
	}


	Norm(ch_div_K,height_in,width_in,
			in,14,
			//&W[0],14,
			&norm_out[0],
			out,14
		);

//	for(int cin=0;cin<ch_div_K;cin++)
//	{
//
//		for(int i=0;i<height_in;i++)
//		{
//			for(int j=0;j<width_in;j++)
//			{
//				dtype_stream tp=out.read();
//				for(int k=0;k<K;k++)
//					std::cout<<"out["<<i<<"]["<<j<<"]["<<cin*K+k<<"]="<<tp.data.range(k*16+15,k*16)<<std::endl;
//			}
//		}
//	}

	for(int cin=0;cin<ch_div_K;cin++)
	{
		for(int k=0;k<K;k++)
			std::cout<<"norm["<<cin*K+k<<"]="<<norm_out[cin].range(32*k+31,32*k)<<std::endl;
	}

	return 0;
}

