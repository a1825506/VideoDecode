package com.videodecode;

public class H264Decoder {
	
	  
    static{
    	System.loadLibrary("ffmpeg");
    	System.loadLibrary("H264Decoder");
    }
    
    public native int InitDecoder(int den,int width,int height);
    
    public native int nativeH264decode(byte[] infrm,int len,byte[] outfrm,int outlen,int nWidth, int nHeight);

    public native void UnInitDecoder();
}
