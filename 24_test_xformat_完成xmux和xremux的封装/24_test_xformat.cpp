#include <iostream>
#include <thread>
#include <vector>
using namespace std;
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
#include "xdecode.h"
#include "xvideo_view.h"
#include "xdemux.h"
#include "xmux.h"

//预处理指令导入库
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")//使用av_strerror需要添加这个库
#pragma comment(lib,"avcodec.lib")


#define CERR(err) {if(!PrintErr(err))return -1;}

int main(int argc, char* argv[])
{
	//打开媒体文件
	const char* url = "v1080.mp4";

	XDemux demux;
	auto demux_c = demux.Open(url);
	demux.set_c(demux_c);

	const char* out_url = "outaa.mp4";
	XMux mux;
	auto mux_c = mux.Open(out_url);
	mux.set_c(mux_c);

	std::vector<int> stream_map;
	if (demux_c)
	{
		stream_map.assign(demux_c->nb_streams, -1);
	}

	auto mvs = mux_c->streams[mux.video_index()]; //视频流信息
	mux.video_index();
	auto mas = mux_c->streams[mux.audio_index()]; //视频流信息
	mux.audio_index();
	if (demux.audio_index() >= 0)
	{
		//mas = mux_c->streams[mux.audio_index()];
		mas->time_base.den = demux.audio_time_base().den;
		mas->time_base.num = demux.audio_time_base().num;
		demux.CopyPara(demux.audio_index(), mas->codecpar);
		if (demux.audio_index() < stream_map.size())
		{
			stream_map[demux.audio_index()] = mux.audio_index();
		}
	}

	if (demux.video_index() >= 0)
	{
		//mvs = mux_c->streams[mux.video_index()];
		mvs->time_base.den = demux.video_time_base().den;
		mvs->time_base.num = demux.video_time_base().num;
		demux.CopyPara(demux.video_index(), mvs->codecpar);
		if (demux.video_index() < stream_map.size())
		{
			stream_map[demux.video_index()] = mux.video_index();
		}
	}


	mux.WriteHeader();

	AVPacket pkt;
	av_init_packet(&pkt);
	
	for (;;)
	{
		if (!demux.Read(&pkt))
		{
			break;
		}

		//if (pkt.stream_index >= stream_map.size())
		//{
		//	av_packet_unref(&pkt);
		//	continue;
		//}

		//int out_stream_index = stream_map[pkt.stream_index];
		//if (out_stream_index < 0)
		//{
		//	av_packet_unref(&pkt);
		//	continue;
		//}

		//AVRational in_tb = demux_c->streams[pkt.stream_index]->time_base;
		//AVRational out_tb = mux_c->streams[out_stream_index]->time_base;
		//av_packet_rescale_ts(&pkt, in_tb, out_tb);
		//pkt.stream_index = out_stream_index;

		pkt.pos = -1;
		//写入音视频会清理pkt，所以下面不用再清理
		mux.Write(&pkt);
		av_packet_unref(&pkt);
	}
	//一定要写入结尾，否则视频播放不了
	mux.WriteTrailer();
	demux.set_c(nullptr);
	mux.set_c(nullptr);
	return 0;
}