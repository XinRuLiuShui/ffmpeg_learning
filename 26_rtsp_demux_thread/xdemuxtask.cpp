#include "xdemuxtask.h"
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
		Next(&pkt);
		av_packet_unref(&pkt);
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
	return true;
}
