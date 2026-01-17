#include "xdecode_task.h"
#include "xtools.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
using namespace std;
bool XDecodeTask::Open(AVCodecParameters* para)
{
	if (!para)
	{
		LOGERROR("XDecodeTask::Open:para is null!");
		return false;
	}
	unique_lock<mutex> lock(mtx_);
	auto c = decode_.Creat(para->codec_id, false);
	if (!c)
	{
		LOGERROR("XDecodeTask::Open:c is null!");
		return false;
	}
	avcodec_parameters_to_context(c, para);
	decode_.set_c(c);
	if (!decode_.Open())
	{
		LOGERROR("XDecodeTask::Open:decode_.Open() failed!");
		return false;
	}
	LOGINFO("decode_.Open() success");
    return true;
}

void XDecodeTask::Do(AVPacket* pkt)
{
	cout << "#" << flush;
	/*if (!pkt || pkt->stream_index != 0) {
		return;
	}*/
	//if (!pkt || pkt->stream_index != video_stream_index_) {
	//	return;
	//}
	if (!pkt || pkt->stream_index != stream_index_) {
		return;
	}
	unique_lock<mutex> lock(mtx_);
	pkt_list_.Push(pkt);
}

void XDecodeTask::Main()
{
	{
		unique_lock<mutex> lock(mtx_);
		if (!frame_)
		{
			frame_ = av_frame_alloc();
		}
	}

	while (!is_exit_)
	{
		auto pkt = pkt_list_.Pop();
		if (!pkt)
		{
			this_thread::sleep_for(1ms);
			continue;
		}
		

		bool re = decode_.Send(pkt);
		av_packet_free(&pkt);
		if (!re)
		{
			this_thread::sleep_for(1ms);
			continue;
		}
		{
			unique_lock<mutex> lock(mtx_);
			if (decode_.Recv(frame_,false))
			{
				cout << "@" << flush;
				need_view_ = true;
				//frame_->format;
			}
			if (is_frame_cache_)
			{
				auto f = av_frame_alloc();
				av_frame_ref(f, frame_);
				frames_.push_back(f);
			}
		}
		this_thread::sleep_for(1ms);
	}

	{
		unique_lock<mutex> lock(mtx_);
		if (frame_)
		{
			av_frame_free(&frame_);
		}
	}
}

AVFrame* XDecodeTask::GetFrame()
{
	unique_lock<mutex> lock(mtx_);
	if (is_frame_cache_)
	{
		if (frames_.empty())
		{
			return nullptr;
		}
		auto f = frames_.front();
		frames_.pop_front();
		return f;
	}
	if (!need_view_ || !frame_	|| !frame_->buf[0])
	{
		return nullptr;
	}
	auto f = av_frame_alloc();
	auto re = av_frame_ref(f, frame_);
	if (re != 0)
	{
		av_frame_free(&f);
		PrintErr(re);
		return nullptr;
	}
	need_view_ = false;
	return f;
}


