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

	//使用说明
	string useage = "24_test_xformat 输入文件 输出文件 开始时间(秒) 结束时间(秒) \n";
	useage += "24_test_xformat v1080.mp4 test_out.mp4 10 10";
	cout << useage << endl;

	if (argc < 3)
	{
		return -1;
	}

	string in_file = argv[2];
	string out_file = argv[3];

	int start_sec = 0;
	int end_sec = 0;
	long long start_pts = 0;
	long long end_pts = 0;

	double audio_start_sec = 0;
	double audio_duration_sec = 0;
	double audio_end_sec = 0;
	long long audio_start_pts = 0;
	long long audio_end_pts = 0;

	if (argc > 3)
	{
		start_sec = atoi(argv[4]);
		audio_start_sec = atoi(argv[4]);
	}

	if (argc > 4)
	{
		end_sec = atoi(argv[5]);
		audio_end_sec = atoi(argv[5]);
	}

	XDemux demux;
	auto demux_c = demux.Open(in_file.c_str());
	demux.set_c(demux_c);


	XMux mux;
	auto mux_c = mux.Open(out_file.c_str());
	mux.set_c(mux_c);


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

	}

	if (demux.video_index() >= 0)
	{
		//mvs = mux_c->streams[mux.video_index()];
		mvs->time_base.den = demux.video_time_base().den;
		mvs->time_base.num = demux.video_time_base().num;
		demux.CopyPara(demux.video_index(), mvs->codecpar);
	}


	if (start_sec && demux.video_index() >= 0 && demux.video_time_base().num > 0)
	{
		double t = (double)demux.video_time_base().den / (double)demux.video_time_base().num;
		start_pts = start_sec * t;
		end_pts = end_sec * t;
		demux.Seek(start_pts, demux.video_index());
	}

	if (start_sec && demux.audio_index() >= 0 && demux.audio_time_base().num > 0)
	{
		double audio_t = (double)demux.audio_time_base().den / (double)demux.audio_time_base().num;
		audio_start_pts = audio_start_sec * audio_t;
		audio_end_pts = audio_end_sec * audio_t;
		demux.Seek(audio_start_pts, demux.audio_index());
	}


	mux.WriteHeader();

	AVPacket pkt;
	av_init_packet(&pkt);
	
	int audio_count = 0;
	int video_count = 0;
	double total_sec = 0;
	for (;;)
	{
		if (!demux.Read(&pkt))
		{
			break;
		}

		//需要判断是否为视频信息
		if (pkt.stream_index == demux.video_index())
		{
			
			if (end_pts > 0 && pkt.pts >= end_pts)
			{
				break;
			}
			video_count++;
			
			total_sec += pkt.duration * (double)((double)demux.video_time_base().num / (double)demux.video_time_base().den);
			auto offset = start_pts;
			mux.RescaleTime(&pkt, offset, demux.video_time_base());
		}

		if (pkt.stream_index == demux.audio_index())
		{
			
			if (audio_end_pts > 0 && pkt.pts >= audio_end_pts)
			{
				break;
			}
			audio_count++;
			auto offset = audio_start_pts;
			mux.RescaleTime(&pkt, offset, demux.audio_time_base());
		}

		

		//写入音视频会清理pkt，所以下面不用再清理
		mux.Write(&pkt);
		av_packet_unref(&pkt);
	}
	cout << "video_count is " << video_count << endl;
	cout << "audio_count is " << audio_count << endl;
	cout << "total_sec is " << total_sec << endl;
	//一定要写入结尾，否则视频播放不了
	mux.WriteTrailer();
	demux.set_c(nullptr);
	mux.set_c(nullptr);
	return 0;
}