#pragma once
#include "xtools.h"
//封装和解封装的基类
#include <iostream>
#include <mutex>

struct XRational {
	int num; ///< Numerator
	int den; ///< Denominator
};

struct AVFormatContext;
struct AVCodecParameters;
struct AVPacket;
struct AVCodecContext;

class XCODEC_API XFormat
{
public:
	//设置上下文，并且清理上次保存的值，如果传NULL的话，相当于关闭上下文
	void set_c(AVFormatContext* c);
	// 参数拷贝
	bool CopyPara(int stream_index, AVCodecParameters* dst);
	bool CopyPara(int stream_index, AVCodecContext* dst);

	//复制视频参数，返回智能指针
	std::shared_ptr<XPara> CopyVideoPara();
	//复制音频参数，返回智能指针
	std::shared_ptr<XPara> CopyAudioPara();

	int audio_index() { return audio_index_; };
	int video_index() { return video_index_; };

	XRational video_time_base() { return  video_time_base_; }
	XRational audio_time_base() { return  audio_time_base_; }

	int video_codec_id() { return video_codec_id_; }

	//根据timebase换算时间
	bool RescaleTime(AVPacket* pkt, long long offset_pts, XRational time_base);
	bool RescaleTime(AVPacket* pkt, long long offset_pts, AVRational* time_base);

	// pts dts duration转化成毫秒计算
	long long RescaleToMs(long long pts, int index);

	void set_time_out_ms(int ms);

	bool IsTimeout()
	{
		if (NowMs() - last_time_ >= time_out_ms_)
		{
			last_time_ = NowMs();
			is_connected_ = false;
			return true;
		}
		return false;
	}
	bool is_connected() { return is_connected_; }
protected:
	int time_out_ms_ = 0;
	long long last_time_ = 0;//上次接收到数据的时间
	bool is_connected_ = false;

	AVFormatContext* fmt_ctx_ = nullptr; 
	std::mutex mtx_;

	//音频和视频在stream中的索引
	int audio_index_ = 1;
	int video_index_ = 0;
	XRational video_time_base_ = { 1,25 };
	XRational audio_time_base_ = { 1,9000 };
	int video_codec_id_ = 0;	//编码器id
};

