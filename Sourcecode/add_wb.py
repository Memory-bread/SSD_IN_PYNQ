from pynq import Overlay
import numpy as np
from pynq import Xlnk
import struct
from numpy import unravel_index
import heapq 
from PIL import Image,ImageFont, ImageDraw
K=8


def Load_Weight_From_File(weight,file):
    with open(file,'rb') as fp:
        for i in range(np.shape(weight)[0]):
            for j in range(np.shape(weight)[1]):
                for k in range(np.shape(weight)[2]):
                    for l in range(np.shape(weight)[3]):
                        for m in range(np.shape(weight)[4]):
                            dat=fp.read(2)
                            a=struct.unpack("h",dat)
                            #print(a[0])
                            weight[i][j][k][l][m]=a[0]

def Load_Bias_From_File(bias,file):
    with open(file,'rb') as fp:
        for i in range(np.shape(bias)[0]):
            dat=fp.read(2)
            a=struct.unpack("h",dat)
            bias[i]=a[0]
         
        
        
def Run_Pool_Soft_padding(ch,kx,ky,feature_in,feature_out):
     for i in range(ch):
            for j in range(feature_out.shape[1]):
                for k in range(feature_out.shape[2]):
                    tp=-32768;
                    for ii in range(ky):
                        for jj in range(kx):
                            row=j-1+ii
                            col=k-1+jj
                            if not (row<0 or col<0 or row>=feature_in.shape[1] or col>=feature_in.shape[2]):
                                dat=feature_in[i//K][row][col][i%K]
                                if(dat>tp):
                                    tp=dat
                    feature_out[i//K][j][k][i%K]=tp 
                            

                    
def Run_Conv_dilation(chin,chout,kx,ky,sx,sy,mode,relu_en,feature_in,feature_in_precision,weight,weight_precision,bias,bias_precision,feature_out,feature_out_precision):     
    if(mode==0):
        pad_x=0
        pad_y=0
    else:
        pad_x=6
        pad_y=6      
    for i in range(chout):
        for j in range(feature_out.shape[1]):
            for k in range(feature_out.shape[2]):
                print("%d[%d][%d]",i,j,k)
                sum=np.int64(0)
                for c in range(chin):
                    for ii in range(ky):
                        for jj in range(kx):
                            row=j*sy-pad_y+ii*6
                            col=k*sx-pad_x+jj*6
                            if not (row<0 or col<0 or row>=feature_in.shape[1] or col>=feature_in.shape[2]):
                                dat=feature_in[c//K][row][col][c%K]
                                wt=weight[i][ii][jj][c//K][c%K]
                                #print("%d %d=%d, wt=%d "%(row,col,dat,wt))
                                sum=sum+int(dat)*int(wt)
                res=(sum>>(feature_in_precision+weight_precision-feature_out_precision))+(bias[i]>>(bias_precision-feature_out_precision))
                if(relu_en&res<0):
                    res=0
                if(res>32767):
                    res=32767
                else:
                    if(res<-32768):
                        res=32768
                feature_out[i//K][j][k][i%K]=res  

def ssd_correct_boxes(top, left, bottom, right, input_shape, image_shape):
    new_shape = image_shape*np.min(input_shape/image_shape)

    offset = (input_shape-new_shape)/2./input_shape
    scale = input_shape/new_shape

    box_yx = np.concatenate(((top+bottom)/2,(left+right)/2),axis=-1)
    box_hw = np.concatenate((bottom-top,right-left),axis=-1)

    box_yx = (box_yx - offset) * scale
    box_hw *= scale

    box_mins = box_yx - (box_hw / 2.)
    box_maxes = box_yx + (box_hw / 2.)
    boxes =  np.concatenate([
        box_mins[:, 0:1],
        box_mins[:, 1:2],
        box_maxes[:, 0:1],
        box_maxes[:, 1:2]
    ],axis=-1)
    print(np.shape(boxes))
    boxes *= np.concatenate([image_shape, image_shape],axis=-1)
    return boxes
                
                
                
# def class_cal(arr,max_N,class_num):
#     for i in range(arr.shape[0]):
#         for j in range(arr.shape[1]):
#             for k in range(arr.shape[2]):
#                 for l in range(arr.shape[3]):
#                     if((i*8+l)%21==0):
#                         arr[i][j][k][l]=0
# idx = np.argsort(arr.ravel())[:-max_N-1:-1] 
# sort_idx=np.column_stack(np.unravel_index(idx, arr.shape))
# for i in range(max_N):
#     box_class=(sort_idx[i][0]*8+sort_idx[i][3])%21-1
#     class_num.append(box_class)                    