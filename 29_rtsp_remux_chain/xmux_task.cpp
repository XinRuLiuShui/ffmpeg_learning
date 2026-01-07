#include "xmux_task.h"
#include "xtools.h"
using namespace std;

void XMuxTask::Main()
{
	mux_.WriteHeader();
	while (!is_exit_)
	{
		auto pkt = pkt_list_.Pop();
		if (!pkt)
		{
			this_thread::sleep_for(1ms);
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
