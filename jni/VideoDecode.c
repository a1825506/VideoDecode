#include <stdio.h>
#include <time.h>

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"

#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(=_=)", format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf("(>_<) " format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf("(^_^) " format "\n", ##__VA_ARGS__)
#endif

static AVCodec *codec = NULL;
static AVCodecContext *codec_ctx= NULL;
static AVFrame *frame = NULL;
static AVPacket avpkt;

JNIEXPORT jint JNICALL Java_com_videodecode_H264Decoder_InitDecoder
  (JNIEnv *env, jobject obj,jint den,jint width,jint height){

	    avcodec_register_all();
	    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
	    if(!codec)
	    {
	        printf("avcodec_find_decoder fail\n");
	        return -1;
	    }
	    codec_ctx = avcodec_alloc_context3(codec);
	        if(!codec_ctx)
	    {
	        printf("avcodec_alloc_context3 fail\n");
	        return -1;
	        }
	    else
	    {
	        codec_ctx->time_base.num = 1;
	        codec_ctx->frame_number = 1; //每包一个视频帧
	        codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
	        codec_ctx->pix_fmt =AV_PIX_FMT_YUV420P;
	        codec_ctx->bit_rate = 0;
	        codec_ctx->time_base.den = den;//帧率
	        codec_ctx->width = width;//视频宽
	        codec_ctx->height = height;//视频高
	    }
	    if(avcodec_open2(codec_ctx, codec, NULL) < 0)
	    {
	        printf("avcodec_open2 fail\n");
	        return -1;
	    }
	    frame = av_frame_alloc();
	    if(!frame)
	    {
	        printf("av_frame_alloc fail\n");
	        return -1;
	    }
	    av_init_packet(&avpkt);
	    return 0;
}
JNIEXPORT jint JNICALL Java_com_videodecode_H264Decoder_nativeH264decode

  (JNIEnv *env, jobject obj,unsigned char *inputbuf, int inputbufsize,unsigned char *outputbuf,int *outputbufsize, int *nWidth, int *nHeight){

	 avpkt.size = inputbufsize;

	 avpkt.data = inputbuf;

	 while(avpkt.size > 0){

	     int got_frame = 0;

		int len = avcodec_decode_video2(codec_ctx, frame, &got_frame, &avpkt);

		if(len < 0)
		{
		     printf("Error while decoding frames\n");
			 return len;
		}
		if(got_frame)
		{

		   *nWidth =codec_ctx->width;

		   *nHeight =codec_ctx->height;

		   if(codec_ctx->width!=0 && codec_ctx->height!=0)
		     {*outputbufsize=codec_ctx->width*codec_ctx->height*3/2;}
		   else
		     {*outputbufsize=0;}
		   pgm_save2(frame->data[0],
				     frame->linesize[0],
				     codec_ctx->width,
					 codec_ctx->height,outputbuf);
		   pgm_save2(frame->data[1],
				   frame->linesize[1],
				   codec_ctx->width/2,
				   codec_ctx->height/2,
				   outputbuf +codec_ctx->width * codec_ctx->height);
		     pgm_save2(frame->data[2],
		    		 frame->linesize[2],
					 codec_ctx->width/2,
					 codec_ctx->height/2,
					 outputbuf +codec_ctx->width * codec_ctx->height*5/4);

		}

	    }
}
void pgm_save2(unsigned char *buf, int wrap, int xsize, int ysize, uint8_t *pDataOut)
    {
        int i;
        for(i = 0; i < ysize; i++)
        {
            memcpy(pDataOut+i*xsize, buf + /*(ysize-i)*/i * wrap, xsize);
        }
    }

JNIEXPORT void JNICALL Java_com_videodecode_H264Decoder_UnInitDecoder(JNIEnv *env, jobject obj){

	avcodec_close(codec_ctx);
	 av_free(codec_ctx);
	 av_free(frame->data[0]);
	 av_free(frame);
}



