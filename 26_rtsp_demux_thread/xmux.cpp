#include "xmux.h"
#include "xdemux.h"
#include "xcodec.h"
#include <iostream>
#include <thread>
using namespace std;
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
#define CERR(err) {if(!PrintErr(err))return 0;}

AVFormatContext* XMux::Open(const char* url)
{
	AVFormatContext* ec = nullptr;
	auto re = avformat_alloc_output_context2(&ec, NULL, NULL, url);
	CERR(re);

	//音频流和视频流
	//这里类型必须设置，否则会报错，无法运行

	//音视频流创建顺序必须要跟在format里面设置的一样，否则会报错
	//例如在format里面设置视频为0，音频为1，这个时候就需要先创建视频流，再创建音频流，否则会导致视频生成错误
	AVStream* out_video_stream = avformat_new_stream(ec, nullptr);
	out_video_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
	AVStream* out_audio_stream = avformat_new_stream(ec, nullptr);
	out_audio_stream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
	

	//打开输出io
	re = avio_open(&ec->pb, url, AVIO_FLAG_WRITE);
	CERR(re);

	return ec;
}

bool XMux::WriteHeader()
{
	unique_lock<mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		return false;
	}
	auto re = avformat_write_header(fmt_ctx_, NULL);
	CERR(re);
	//打印封装信息
	av_dump_format(fmt_ctx_, 0, fmt_ctx_->url,
		1	//0表示上下文是输入，1表示上下文是输出
	);
	return true;
}

bool XMux::Write(AVPacket* pkt)
{
	unique_lock<mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		return false;
	}
	auto re = av_interleaved_write_frame(fmt_ctx_, pkt);
	CERR(re);
	return true;
}

bool XMux::WriteTrailer()
{
	unique_lock<mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		return false;
	}
	av_interleaved_write_frame(fmt_ctx_, nullptr);//写入排序缓冲
	auto re = av_write_trailer(fmt_ctx_);
	CERR(re);
	
	return true;
}
