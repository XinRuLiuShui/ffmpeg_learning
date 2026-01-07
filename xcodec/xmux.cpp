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

AVFormatContext* XMux::Open(const char* url,
	AVCodecParameters* video_para,
	AVCodecParameters* audio_para)
{
	AVFormatContext* ec = nullptr;
	auto re = avformat_alloc_output_context2(&ec, NULL, NULL, url);
	CERR(re);

	//音频流和视频流
	//这里类型必须设置，否则会报错，无法运行

	//音视频流创建顺序必须要跟在format里面设置的一样，否则会报错
	//例如在format里面设置视频为0，音频为1，这个时候就需要先创建视频流，再创建音频流，否则会导致视频生成错误
	
	if (video_para)
	{
		AVStream* out_video_stream = avformat_new_stream(ec, nullptr);
		avcodec_parameters_copy(out_video_stream->codecpar, video_para);
	}

	if (audio_para)
	{
		AVStream* out_audio_stream = avformat_new_stream(ec, nullptr);
		avcodec_parameters_copy(out_audio_stream->codecpar, audio_para);
	}
	

	//打开输出io
	re = avio_open(&ec->pb, url, AVIO_FLAG_WRITE);
	CERR(re);
	LOGINFO("start av_dump_format");
	av_dump_format(ec, 0, url, 1);
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

	begin_video_pts = -1;
	begin_audio_pts = -1;

	return true;
}

bool XMux::Write(AVPacket* pkt)
{
	unique_lock<mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		return false;
	}
	
	//没读取到pts,重构时考虑通过duration计算
	if (pkt->pts == AV_NOPTS_VALUE)
	{
		pkt->pts = 0;
		pkt->dts = 0;
	}

	if (pkt->stream_index == video_index_)
	{
		pkt->stream_index = 0;
		if (begin_video_pts < 0)
		{
			begin_video_pts = pkt->pts;
		}
		lock.unlock();
		RescaleTime(pkt, begin_video_pts, src_video_time_base_);
		lock.lock();
	}
	if (pkt->stream_index == audio_index_)
	{
		pkt->stream_index = 1;
		if (begin_audio_pts < 0)
		{
			begin_audio_pts = pkt->pts;
		}
		lock.unlock();
		RescaleTime(pkt, begin_audio_pts, src_audio_time_base_);
		lock.lock();
	}
	cout << pkt->pts << flush;
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

void XMux::set_src_video_time_base(AVRational* tb)
{
	if (!tb)
	{
		return;
	}
	unique_lock<mutex> lock(mtx_);
	if (!src_video_time_base_)
	{
		src_video_time_base_ = new AVRational();
	}
	*src_video_time_base_ = *tb;
}

void XMux::set_src_audio_time_base(AVRational* tb)
{
	if (!tb)
	{
		return;
	}
	unique_lock<mutex> lock(mtx_);
	if (!src_audio_time_base_)
	{
		src_audio_time_base_ = new AVRational();
	}
	*src_audio_time_base_ = *tb;
}

XMux::~XMux()
{
	unique_lock<mutex> lock(mtx_);
	if (src_video_time_base_)
	{
		delete src_video_time_base_;
		src_video_time_base_ = nullptr;
	}
	if (src_audio_time_base_)
	{
		delete src_audio_time_base_;
		src_audio_time_base_ = nullptr;
	}
}
