#include "xplayer.h"
#include "xaudio_play.h"
#include <iostream>
using namespace std;

bool XPlayer::Open(const char* url, void* winid)
{
	if (!demux_task_.Open(url))
	{
		cerr << "demux_task Open failed" << endl;
		return false;
	}
	auto vpara = demux_task_.CopyVideoPara();
	auto apara = demux_task_.CopyAudioPara();

	if (vpara)
	{
		if (!video_decode_task_.Open(vpara->para))
		{
			cerr << "video_decode_task_ Open failed" << endl;
			return false;
		}

		video_decode_task_.set_video_stream_index(demux_task_.video_index());
		video_decode_task_.set_audio_stream_index(demux_task_.audio_index());
		video_decode_task_.set_stream_index(demux_task_.video_index());

		if (!view_)
		{
			view_ = XVideoView::Creat(XVideoView::SDL);
		}
		view_->set_win_id(winid);
		if (!view_->Init(vpara->para))
		{
			return false;
		}
	}
	
	if (apara)
	{
		if (!audio_decode_task_.Open(apara->para))
		{
			cerr << "audio_decode_task_ Open failed" << endl;
			return false;
		}
		audio_decode_task_.set_video_stream_index(demux_task_.video_index());
		audio_decode_task_.set_audio_stream_index(demux_task_.audio_index());
		audio_decode_task_.set_stream_index(demux_task_.audio_index());
		
		//设置音频缓冲
		audio_decode_task_.set_is_frame_cache(true);

		XAudioPlay::Instance()->Open(apara->para);

	}
	else
	{
		demux_task_.set_syn_type(XSYN_VIDEO);
	}

	//把解封装的数据传到此类
	demux_task_.set_next(this);

	


    return true;
}

void XPlayer::Main()
{
}

void XPlayer::Do(AVPacket* pkt)
{
	if (video_decode_task_.video_stream_index() != -1)
	{
		video_decode_task_.Do(pkt);
	}
	if (audio_decode_task_.audio_stream_index() != -1)
	{
		audio_decode_task_.Do(pkt);
	}
}

void XPlayer::Start(void)
{
	demux_task_.Start();
	if (video_decode_task_.video_stream_index() != -1)
	{
		video_decode_task_.Start();
	}
	if (audio_decode_task_.audio_stream_index() != -1)
	{
		audio_decode_task_.Start();
	}
	XThread::Start();
}

void XPlayer::Update(void)
{
	//渲染视频
	if (view_)
	{
		auto f = video_decode_task_.GetFrame();
		view_->DrawFrame(f);
		XFreeFrame(&f);
	}

	auto f = audio_decode_task_.GetFrame();
	if (!f)
	{
		return;
	}
	XAudioPlay::Instance()->Push(f);
	XFreeFrame(&f);
}
