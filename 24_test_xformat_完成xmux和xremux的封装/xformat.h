#pragma once
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
class XFormat
{
public:
	//设置上下文，并且清理上次保存的值，如果传NULL的话，相当于关闭上下文
	void set_c(AVFormatContext* c);
	// 参数拷贝
	bool CopyPara(int stream_index, AVCodecParameters* dst);

	int audio_index() { return audio_index_; };
	int video_index() { return video_index_; };

	XRational video_time_base() { return  video_time_base_; }
	XRational audio_time_base() { return  audio_time_base_; }
protected:
	AVFormatContext* fmt_ctx_ = nullptr; 
	std::mutex mtx_;

	//音频和视频在stream中的索引
	int audio_index_ = 1;
	int video_index_ = 0;
	XRational video_time_base_ = { 1,25 };
	XRational audio_time_base_ = { 1,9000 };
};

