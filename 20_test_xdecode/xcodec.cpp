#include "xcodec.h"
#include <iostream>
#include <fstream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")//使用av_strerror需要添加这个库

using namespace std;

void PrintErr(int re)
{
	if (re != 0)
	{
		char buf[1024];
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << buf << endl;
	}
}

AVCodecContext* XCodec::Creat(int codec_id, bool isEncode)
{
	AVCodec* codec;
	if (isEncode)
	{
		//1.找到编码器
		codec = avcodec_find_encoder((AVCodecID)codec_id);
	}
	else
	{
		codec = avcodec_find_decoder((AVCodecID)codec_id);
	}

	
	if (!codec)
	{
		cerr << "codec no found!" << endl;
		return nullptr;
	}
	//2.编码上下文
	auto c = avcodec_alloc_context3(codec);
	if (!c)
	{
		cerr << "avcodec_alloc_context3 failed!" << endl;
		return nullptr;
	}


	//设置默认参数
	//3.设定上下文参数 

	//帧时间戳的时间单位 pts*time_base=播放时间（秒）
	c->time_base = { 1,25 };//分子和分母

	c->pix_fmt = AV_PIX_FMT_YUV420P;//元数据像是格式要与编码算法所支持的一致
	c->thread_count = 6;//编码线程数
	return c;
}

void XCodec::set_c(AVCodecContext* c)
{
	unique_lock<mutex>lock(mux_);
	if (c_)
	{
		//释放上下文
		avcodec_free_context(&c_);
	}

	this->c_ = c;
}

bool XCodec::SetOpt(const char* key, const char* val)
{
	unique_lock<mutex>lock(mux_);
	if (!c_)
	{
		return false;
	}

	int opt_re = av_opt_set(c_->priv_data, key, val, 0);
	if (opt_re != 0)
	{
		cout << "av_opt_set failed!" << endl;
		return false;
	}
	return true;
}

bool XCodec::SetOpt(const char* key, int val)
{
	unique_lock<mutex>lock(mux_);
	if (!c_)
	{
		return false;
	}
	int opt_re = av_opt_set_int(c_->priv_data, key, (int64_t)val, 0);		//最快速度
	if (opt_re != 0)
	{
		cout << "av_opt_set failed!" << endl;
		PrintErr(opt_re);
		return false;
	}
	return true;
}

bool XCodec::Open()
{
	unique_lock<mutex>lock(mux_);
	if (!c_)
	{
		return false;
	}
	//打开编码上下文。
	int re = avcodec_open2(c_, NULL, NULL);//第2个参数codec在创建的时候已经配置了，所以这里传空
	if (re != 0)
	{
		cout << "avcodec_open2 failed!" << endl;
		PrintErr(re);
		return false;
	}
	return true;
}

AVFrame* XCodec::CreatFrame()
{
	unique_lock<mutex>lock(mux_);
	if (!c_)
	{
		return nullptr;
	}
	AVFrame* frame = av_frame_alloc();
	frame->width = c_->width;
	frame->height = c_->height;
	frame->format = c_->pix_fmt;
	//生成AVframe空间,默认对齐参数,有可能会报错 
	int re = av_frame_get_buffer(frame, 0);
	if (re != 0)
	{
		PrintErr(re);
		//生成空间失败同样要释放 
		av_frame_free(&frame);
		return nullptr;
	}
	return frame;
}
