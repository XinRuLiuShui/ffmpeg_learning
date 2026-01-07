#include "xdemux.h"
#include "xcodec.h"
#include <iostream>
#include <thread>
using namespace std;
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
#define CERR(err) {if(!PrintErr(err))return 0;}
AVFormatContext* XDemux::Open(const char* url)
{
	AVFormatContext* fmt_ctx = nullptr;
	int re = avformat_open_input(&fmt_ctx, url, nullptr, nullptr);
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
	return true;
}
