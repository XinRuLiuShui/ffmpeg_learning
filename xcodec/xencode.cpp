#include "xencode.h"

#include <iostream>
#include <fstream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")//使用av_strerror需要添加这个库
using namespace std;



AVPacket* XEncode::Encode(const AVFrame* frame)
{
	unique_lock<mutex>lock(mux_);
	if (!c_ || !frame)
	{
		return nullptr;
	}
	auto pkt = av_packet_alloc();

	//将原始数据发送给线程进行压缩
	int re = avcodec_send_frame(c_, frame);
	if (re != 0)
	{
		return nullptr;
	}
	//调用之后会立刻返回，但不代表已经压缩完成，需要重复询问
	//每次调用会重新生成pkt的空间，所以调用完之后需要释放
	re = avcodec_receive_packet(c_, pkt);
	if (re == 0)
	{
		return pkt;
	}
	av_packet_free(&pkt);
	if (re == AVERROR(EAGAIN) || re == AVERROR_EOF)
	{
		return nullptr;
	}
	if (re < 0)
	{
		PrintErr(re);
	}
	return nullptr;
}


std::vector<AVPacket*> XEncode::End()
{
	std::vector<AVPacket*> res;
	unique_lock<mutex>lock(mux_);
	if (!c_)
	{
		return res;
	}
	
	//发送NULL，获取缓冲
	int re = avcodec_send_frame(c_, NULL);
	if (re != 0)
	{
		return res;
	}
	while (re >= 0)
	{
		auto pkt = av_packet_alloc();
		//调用之后会立刻返回，但不代表已经压缩完成，需要重复询问
		//每次调用会重新生成pkt的空间，所以调用完之后需要释放
		re = avcodec_receive_packet(c_, pkt);

		if (re != 0)
		{
			av_packet_free(&pkt);
			break;
		}
		res.push_back(pkt);
	}
	return res;
}
