#include "xdemux_task.h"
#include <iostream>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

using namespace std;
void XDemuxTask::Main()
{
	AVPacket pkt;
	while (!is_exit_)
	{
		if (!demux_.is_connected())
		{
			cout << "连接断开，正在重连..." << endl;
			if (!Open(url_, timeout_ms_))
			{
				cout << "重连失败，1秒后重试..." << endl;
				this_thread::sleep_for(1000ms);
				continue;
			}
			cout << "重连成功!" << endl;
		}
		if (!demux_.Read(&pkt))
		{
			cout << "- ";
			this_thread::sleep_for(1ms);
			continue;
		}
		cout << ". ";

		//播放速度控制
		if (syn_type_ == XSYN_VIDEO && pkt.stream_index == demux_.video_index())
		{
			auto period = demux_.RescaleToMs(pkt.duration, pkt.stream_index);
			if (period <= 0)
			{
				period = 40;
			}
			MSleep(period);
		}

		Next(&pkt);
		av_packet_unref(&pkt);
		//AVPacket* newpkt = av_packet_alloc();
		//av_packet_ref(newpkt, &pkt);   // 深拷贝引用计数

		//Next(newpkt);
		//av_packet_unref(&pkt);         // 现在可以安全 unref
		
		this_thread::sleep_for(1ms);
	}

}

bool XDemuxTask::Open(std::string url, int timeout_ms)
{
	LOGDEBUG("XDemuxTask::Open strat");
	demux_.set_c(nullptr);//断开之前的连接，重新设置
	
	// 在Open之前先设置超时时间
	url_ = url;
	timeout_ms_ = timeout_ms;
	demux_.set_time_out_ms(timeout_ms);
	
	auto c = demux_.Open(url.c_str());
	if (!c)
	{
		return false;
	}
	demux_.set_c(c);

	int vindex = demux_.video_index();

	return true;
}

std::shared_ptr<XPara> XDemuxTask::CopyVideoPara()
{
	return demux_.CopyVideoPara();
}

std::shared_ptr<XPara> XDemuxTask::CopyAudioPara()
{
	return demux_.CopyAudioPara();
}
