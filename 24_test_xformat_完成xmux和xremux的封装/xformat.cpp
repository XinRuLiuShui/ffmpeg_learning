#include "xformat.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
#include "xdecode.h"
#include "xvideo_view.h"
using namespace std;
void XFormat::set_c(AVFormatContext* c)
{
	unique_lock<mutex> lock(mtx_);
	if (fmt_ctx_)
	{
		if (fmt_ctx_->oformat)
		{
			if (fmt_ctx_->pb)
			{
				avio_closep(&fmt_ctx_->pb);
			}
			avformat_free_context(fmt_ctx_);
		}
		else if (fmt_ctx_->iformat)
		{
			avformat_close_input(&fmt_ctx_);
		}
		else
		{
			avformat_free_context(fmt_ctx_);
		}
	}

	fmt_ctx_ = c;
	if (!fmt_ctx_)
	{
		return;
	}

	audio_index_ = -1;
	video_index_ = -1;
	for (int i = 0; i < c->nb_streams; i++)
	{
		if (c->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audio_index_ = i;
			audio_time_base_.den = c->streams[i]->time_base.den;
			audio_time_base_.num = c->streams[i]->time_base.num;
		}
		else if (c->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_index_ = i;
			video_time_base_.den = c->streams[i]->time_base.den;
			video_time_base_.num = c->streams[i]->time_base.num;
		}
	}

	

	
}

bool XFormat::CopyPara(int stream_index, AVCodecParameters* dst)
{
	unique_lock<mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		return false;
	}
	if (stream_index < 0 || stream_index > fmt_ctx_->nb_streams)
	{
		return false;
	}
	auto re = avcodec_parameters_copy(dst, fmt_ctx_->streams[stream_index]->codecpar);
	if (re != 0)
	{
		return false;
	}
	return true;
}
