#include "xmux_task.h"
#include "xtools.h"

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

using namespace std;

void XMuxTask::Main()
{
	mux_.WriteHeader();

	//找到关键帧
	//while (!is_exit_)
	//{
	//	AVPacket* pkt = nullptr;
	//	{
	//		unique_lock<mutex> lock(mtx_);
	//		pkt = pkt_list_.Pop();
	//	}
	//	if (!pkt)
	//	{
	//		this_thread::sleep_for(1ms);
	//		//MSleep(1);
	//		continue;
	//	}
	//	//找到关键帧
	//	if (pkt->stream_index == mux_.video_index() && pkt->flags & AV_PKT_FLAG_KEY)
	//	{
	//		mux_.Write(pkt);
	//		av_packet_free(&pkt);
	//		break;
	//	}
	//	//释放非关键帧
	//	av_packet_free(&pkt);
	//}

	while (!is_exit_)
	{
		AVPacket* pkt = nullptr;
		{
			unique_lock<mutex> lock(mtx_);
			pkt = pkt_list_.Pop();
		}
		if (!pkt)
		{
			this_thread::sleep_for(1ms);
			//MSleep(1);
			continue;
		}
		bool re = mux_.Write(pkt);
		av_packet_free(&pkt);
		if (!re)
		{
			this_thread::sleep_for(1ms);
			continue;
		}
		cout << "/" << flush;
		this_thread::sleep_for(1ms);
	}
	mux_.WriteTrailer();
	mux_.set_c(nullptr);
}

bool XMuxTask::Open(const char* url, AVCodecParameters* video_para, AVRational* video_time_base, AVCodecParameters* audio_para, AVRational* audio_time_base)
{
    auto c = mux_.Open(url, video_para, audio_para);
	if (!c)
	{
		return false;
	}
	mux_.set_c(c);
	mux_.set_src_video_time_base(video_time_base);
	mux_.set_src_audio_time_base(audio_time_base);

    return true;
}

void XMuxTask::Do(AVPacket* pkt)
{
	if (!pkt) {
		return;
	}
	unique_lock<mutex> lock(mtx_);
	
	pkt_list_.Push(pkt);
	Next(pkt);
}
