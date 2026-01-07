#include <iostream>
#include <thread>
#include <vector>
using namespace std;
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}
#include "xdemux.h"
#include "xmux.h"
#include "xcodec.h"
#include "xdecode.h"
#include "xencode.h"

//预处理指令导入库
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")//使用av_strerror需要添加这个库
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swscale.lib")//图像缩放库


#define CERR(err) {if(!PrintErr(err))return -1;}

int main(int argc, char* argv[])
{

	//使用说明
	string useage = "24_test_xformat 输入文件 输出文件 开始时间(秒) 结束时间(秒) \n";
	useage += "24_test_xformat v1080.mp4 test_out.mp4 10 10 400 300";
	cout << useage << endl;

	if (argc < 3)
	{
		return -1;
	}
	string a = argv[0]; 
	string b = argv[1];
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

	int width = 0;
	int height = 0;

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

	if (argc > 6)
	{
		width = atoi(argv[6]);
		height = atoi(argv[7]);
	}
	argv[0];
	argv[1];
	argv[2];
	argv[3];
	argv[4];
	argv[5];
	argv[6];
	XDemux demux;
	auto demux_c = demux.Open(in_file.c_str());
	demux.set_c(demux_c);

	//视频编码器
	XEncode ec;
	auto encode_c = ec.Creat(AV_CODEC_ID_H265, true);
	encode_c->pix_fmt = AV_PIX_FMT_YUV420P;


	if (demux.video_index() >= 0)
	{
		if (width <= 0)
			width = demux_c->streams[demux.video_index()]->codecpar->width;
		if (height <= 0)
			height = demux_c->streams[demux.video_index()]->codecpar->height;

		//让编码器时间基、帧率跟输入一致，避免时间戳错导致帧丢失
		auto in_video_stream = demux_c->streams[demux.video_index()];
		encode_c->time_base = in_video_stream->time_base;
		if (in_video_stream->avg_frame_rate.num && in_video_stream->avg_frame_rate.den)
		{
			encode_c->framerate = in_video_stream->avg_frame_rate;
		}
	}
	else
	{
		encode_c->time_base = { 1, 25 };
	}
	encode_c->width = width;
	encode_c->height = height;


	/**************************************************/
	//从输入视频复制关键参数
	//if (demux.video_index() >= 0)
	//{
	//	auto video_stream = demux_c->streams[demux.video_index()];
	//	//复制时间基准
	//	encode_c->time_base = video_stream->time_base;
	//	//复制帧率
	//	encode_c->framerate = video_stream->avg_frame_rate;
	//	//设置比特率（可以根据分辨率调整）
	//	encode_c->bit_rate = demux_c->streams[demux.video_index()]->codecpar->bit_rate;
	//	if (encode_c->bit_rate <= 0)
	//	{
	//		encode_c->bit_rate = 2000000; //默认2Mbps
	//	}
	//	//设置GOP大小
	//	encode_c->gop_size = 25;
	//	encode_c->max_b_frames = 0; //H265可以设置B帧
	//}
	/*****************************************************/
	ec.set_c(encode_c);

	if (!ec.Open())
	{
		cout << "encoder Open Failed!" << endl;
	}


	


	XMux mux;
	auto mux_c = mux.Open(out_file.c_str());
	mux.set_c(mux_c);


	auto mvs = mux_c->streams[mux.video_index()]; //视频流信息

	auto mas = mux_c->streams[mux.audio_index()]; //视频流信息

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
		//demux.CopyPara(demux.video_index(), mvs->codecpar);
		avcodec_parameters_from_context(mvs->codecpar, encode_c);

		
	}


	if (start_sec && demux.video_index() >= 0 && demux.video_time_base().num > 0)
	{
		double t = (double)demux.video_time_base().den / (double)demux.video_time_base().num;
		start_pts = start_sec * t;
		end_pts = end_sec * t;
		/*demux.Seek(start_pts, demux.video_index());*/
	}

	if (start_sec && demux.audio_index() >= 0 && demux.audio_time_base().num > 0)
	{
		double audio_t = (double)demux.audio_time_base().den / (double)demux.audio_time_base().num;
		audio_start_pts = audio_start_sec * audio_t;
		audio_end_pts = audio_end_sec * audio_t;
		/*demux.Seek(audio_start_pts, demux.audio_index());*/
	}

	

	//视频解码器
	XDecode de;
	auto decode_c = de.Creat(demux.video_codec_id(), false);
	demux.CopyPara(demux.video_index(), decode_c);
	de.set_c(decode_c);
	de.Open();
	AVFrame* frame = de.CreatFrame();

	/////////////////////////////////////////////
	//创建缩放用的目标帧
	//AVFrame* scaled_frame = av_frame_alloc();
	//scaled_frame->format = encode_c->pix_fmt;
	//scaled_frame->width = encode_c->width;
	//scaled_frame->height = encode_c->height;
	//av_frame_get_buffer(scaled_frame, 0);
	/////////////////////////////////////////////
	
	//创建图像缩放上下文
	//SwsContext* sws_ctx = nullptr;
	//if (encode_c->width != decode_c->width || encode_c->height != decode_c->height)
	//{
	//	sws_ctx = sws_getContext(
	//		decode_c->width, decode_c->height, decode_c->pix_fmt,  //源
	//		encode_c->width, encode_c->height, encode_c->pix_fmt,  //目标
	//		SWS_BICUBIC, nullptr, nullptr, nullptr);
	//	if (!sws_ctx)
	//	{
	//		cout << "sws_getContext failed!" << endl;
	//		return -1;
	//	}
	//}
	/////////////////////////////////////////////
	

	AVPacket pkt;
	av_init_packet(&pkt);
	
	int audio_count = 0;
	int video_count = 0;
	double total_sec = 0;

	mux.WriteHeader();
	for (;;)
	{
		if (!demux.Read(&pkt))
		{
			break;
		}

		//需要判断是否为视频信息
		if (pkt.stream_index == demux.video_index())
		{
			/*if (end_pts > 0 && pkt.pts >= end_pts)
			{
				av_packet_unref(&pkt);
				break;
			}*/
			
			//调整时间戳偏移
			//auto offset = start_pts;
			auto offset = 0;
			mux.RescaleTime(&pkt, offset, demux.video_time_base());
			//解码视频
			if (!de.Send(&pkt))
			{
				av_packet_unref(&pkt);
				break;
			}
			
			while (de.Recv(frame, false))
			{
				/////////////////////////////////////////////
				//如果需要缩放，先缩放图像
				//AVFrame* encode_frame = frame;
				//if (sws_ctx)
				//{
				//	sws_scale(sws_ctx, frame->data, frame->linesize, 0, decode_c->height,
				//		scaled_frame->data, scaled_frame->linesize);
				//	scaled_frame->pts = frame->pts;
				//	encode_frame = scaled_frame;
				//}

				//auto epkt = ec.Encode(encode_frame);
				/////////////////////////////////////////////
				auto epkt = ec.Encode(frame);
				if (epkt)
				{
					epkt->stream_index = mux.video_index();
					av_packet_rescale_ts(epkt, encode_c->time_base, mvs->time_base);

					cout << ".";
					mux.Write(epkt);
					video_count++;
					av_packet_free(&epkt);
				}
			}
		}

		if (pkt.stream_index == demux.audio_index())
		{
			/*if (audio_end_pts > 0 && pkt.pts >= audio_end_pts)
			{
				break;
			}*/
			audio_count++;
			/*auto offset = audio_start_pts;*/
			auto offset = 0;
			/*mux.RescaleTime(&pkt, offset, demux.audio_time_base());*/
			mux.RescaleTime(&pkt, offset, demux.audio_time_base());
			mux.Write(&pkt);
		}

		av_packet_unref(&pkt);
	}

	//冲刷编码器，写入缓冲区里最后几帧
	for (auto epkt : ec.End())
	{
		epkt->stream_index = mux.video_index();
		av_packet_rescale_ts(epkt, encode_c->time_base, mvs->time_base);
		mux.Write(epkt);
		video_count++;
		av_packet_free(&epkt);
	}

	cout << "video_count is " << video_count << endl;
	cout << "audio_count is " << audio_count << endl;
	cout << "total_sec is " << total_sec << endl;
	//一定要写入结尾，否则视频播放不了
	mux.WriteTrailer();
	

	/////////////////////////////////////////////
	//释放资源
	//if (sws_ctx)
	//{
	//	sws_freeContext(sws_ctx);
	//}
	//av_frame_free(&scaled_frame);
	/////////////////////////////////////////////


	demux.set_c(nullptr);
	mux.set_c(nullptr);
	return 0;
}