#include "xformat.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
#include "xdecode.h"
#include "xtools.h"
using namespace std;

//int (*callback)(void*);
static int TimeoutCallback(void* para)
{
	auto xf = (XFormat*)para;
	if (xf->IsTimeout())
	{
		cout << "TimeoutCallback" << endl;
		return 1;
	}
	
	//正常阻塞
	return 0;
}

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
		is_connected_ = false;
		return;
	}
	is_connected_ = true;

	//记时用于超时判断
	last_time_ = NowMs();

	//设定超时处理回调函数
	if (time_out_ms_ > 0)
	{
		AVIOInterruptCB cb = { TimeoutCallback,this };
		fmt_ctx_->interrupt_callback = cb;
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
			video_codec_id_ = c->streams[i]->codecpar->codec_id;
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

bool XFormat::CopyPara(int stream_index, AVCodecContext* dst)
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

	
	auto re = avcodec_parameters_to_context(dst, fmt_ctx_->streams[stream_index]->codecpar);
	if (re != 0)
	{
		return false;
	}
	return true;
}

std::shared_ptr<XPara> XFormat::CopyVideoPara()
{
	unique_lock<mutex> lock(mtx_);
	shared_ptr<XPara> re;
	int index = video_index();
	if (index < 0 || !fmt_ctx_)
	{
		return nullptr;
	}
	re.reset(XPara::Create());
	*re->time_base = fmt_ctx_->streams[index]->time_base;
	avcodec_parameters_copy(re->para, fmt_ctx_->streams[index]->codecpar);
	return re;
}

bool XFormat::RescaleTime(AVPacket* pkt, long long offset_pts, XRational time_base)
{
	unique_lock<mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		return false;
	}
	auto out_stream = fmt_ctx_->streams[pkt->stream_index];
	AVRational in_timebase;
	in_timebase.den = time_base.den;
	in_timebase.num = time_base.num;
	
	pkt->pts = av_rescale_q_rnd(pkt->pts - offset_pts, in_timebase, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

	pkt->dts = av_rescale_q_rnd(pkt->dts - offset_pts, in_timebase, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

	pkt->duration = av_rescale_q(pkt->duration, in_timebase, out_stream->time_base);
		
	pkt->pos = -1;

	return true;
}


void XFormat::set_time_out_ms(int ms)
{
	unique_lock<mutex> lock(mtx_);
	time_out_ms_ = ms;
	//设定超时处理回调函数
	if (fmt_ctx_)
	{
		AVIOInterruptCB cb = { TimeoutCallback,this };
		fmt_ctx_->interrupt_callback = cb;
	}

}
