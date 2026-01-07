#include "xdemux.h"
#include "xcodec.h"
#include "xtools.h"

#include <iostream>
#include <thread>
using namespace std;
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
#define CERR(err) {if(!PrintErr(err))return 0;}

// 超时回调函数
static int TimeoutCallback(void* para)
{
	auto xf = (XFormat*)para;
	if (xf->IsTimeout())
	{
		cout << "TimeoutCallback in XDemux::Open" << endl;
		return 1;
	}
	//正常阻塞
	return 0;
}

AVFormatContext* XDemux::Open(const char* url)
{
	AVFormatContext* fmt_ctx = nullptr;

	AVDictionary* opts = nullptr;
	//av_dict_set(&opts, "rtsp_transport", "tcp", 0);//修改传输协议为tcp,默认为udp
	av_dict_set(&opts, "stimeout", "3000000", 0); // 3秒超时

	// 先分配context
	fmt_ctx = avformat_alloc_context();
	if (!fmt_ctx)
	{
		return nullptr;
	}

	int re = avformat_open_input(&fmt_ctx, url, nullptr, &opts);
	av_dict_free(&opts);
	CERR(re);

	//获取媒体文件的完整流信息
	re = avformat_find_stream_info(fmt_ctx, nullptr);
	CERR(re);

	//打印封装信息
	av_dump_format(fmt_ctx, 0, url,
		0	//0表示上下文是输入，1表示上下文是输出
	);
    return fmt_ctx;
}

bool XDemux::Read(AVPacket* pkt)
{
	unique_lock<mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		return false;
	}
	auto re = av_read_frame(fmt_ctx_, pkt);
	CERR(re);
	//记时用于超时判断
	last_time_ = NowMs();
	return true;
}

bool XDemux::Seek(long long pts, int stream_index)
{
	unique_lock<mutex> lock(mtx_);
	if (!fmt_ctx_)
	{
		return false;
	}
	int re = av_seek_frame(fmt_ctx_, stream_index, pts, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);
	if (re < 0)
	{
		return false;
	}
	return true;
}
