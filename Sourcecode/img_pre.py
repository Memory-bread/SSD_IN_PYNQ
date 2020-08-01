import numpy as np
from PIL import Image

K=8

def letterbox_image(image, size):
    iw, ih = image.size
    w, h = size
    scale = min(w/iw, h/ih)
    nw = int(iw*scale)
    nh = int(ih*scale)

    image = image.resize((nw,nh), Image.BICUBIC)
    new_image = Image.new('RGB', size, (128,128,128))
    new_image.paste(image, ((w-nw)//2, (h-nh)//2))
    return new_image

def Run_Conv(conv,chin,chout,kx,ky,sx,sy,mode,relu_en,dilation,feature_in,feature_in_precision,weight,weight_precision,bias,bias_precision,feature_out,feature_out_precision):   
    conv.write(0x10,chin)
    conv.write(0x18,feature_in.shape[1])
    conv.write(0x20,feature_in.shape[2])
    conv.write(0x28,chout)
    conv.write(0x30,kx)
    conv.write(0x38,ky)
    conv.write(0x40,sx)
    conv.write(0x48,sy)
    conv.write(0x50,mode)
    conv.write(0x58,relu_en)
    conv.write(0x60,dilation)
    
    conv.write(0x68,feature_in.physical_address)
    conv.write(0x70,feature_in_precision)
    conv.write(0x78,weight.physical_address)
    conv.write(0x80,weight_precision)
    conv.write(0x88,bias.physical_address)
    conv.write(0x90,bias_precision)
    conv.write(0x98,feature_out.physical_address)
    conv.write(0xa0,feature_out_precision)
    #print("conv ip start")
    conv.write(0, (conv.read(0)&0x80)|0x01 ) #start pool IP
    #poll the done bit
    tp=conv.read(0)
    while not((tp>>1)&0x1):
        tp=conv.read(0)
    #print("conv ip done")
    
def Run_Pool(pool,dma,ch,kx,ky,feature_in,feature_out):
    pool.write(0x10,(ch+K-1)//K);
    pool.write(0x18,feature_in.shape[1])
    pool.write(0x20,feature_in.shape[2])
    pool.write(0x28,feature_out.shape[1])
    pool.write(0x30,feature_out.shape[2])
    pool.write(0x38,kx)
    pool.write(0x40,ky)
    #print("start");
    pool.write(0, (pool.read(0)&0x80)|0x01 ) #start pool IP
    dma.recvchannel.transfer(feature_out)
    dma.sendchannel.transfer(feature_in)
    dma.sendchannel.wait();
    #print("send done")
    dma.recvchannel.wait()
    #print("recv done")
    tp=pool.read(0)
    while not((tp>>1)&0x1):
        tp=pool.read(0)
    #print("pool ip done") 