#include <iostream>
#include <thread>
using namespace std;
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
#include "xdecode.h"
#include "xvideo_view.h"

//预处理指令导入库
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")//使用av_strerror需要添加这个库
#pragma comment(lib,"avcodec.lib")

//bool PrintErr(int re)
//{
//	if (re != 0)
//	{
//		char buf[1024];
//		av_strerror(re, buf, sizeof(buf) - 1);
//		cout << buf << endl;
//		return false;
//	}
//	return true;
//}
#define CERR(err) {if(!PrintErr(err))return -1;}

int main(int argc, char* argv[])
{
	//打开媒体文件
	const char* url = "v1080.mp4";

	AVFormatContext* ic = nullptr;
	auto re = avformat_open_input(&ic, url,
		NULL,	//封装器格式 为空自动根据后缀名或者文件头检测
		NULL	//参数设置 rtsp需要设置
	);
	CERR(re);

	//获取媒体信息,无头部文件
	re = avformat_find_stream_info(ic, NULL);
	CERR(re);

	//打印封装信息
	av_dump_format(ic, 0, url,
		0	//0表示上下文是输入，1表示上下文是输出
	);

	AVStream* as = nullptr;
	AVStream* vs = nullptr;
	for (int i = 0; i < ic->nb_streams; i++)
	{
		if (ic->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			as = ic->streams[i];
			cout << "===========音频===========" << endl;
			cout << "sample reat:" << as->codecpar->sample_rate << endl;
		}
		else if (ic->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			vs = ic->streams[i];
			cout << "===========视频===========" << endl;
			cout << "widht:" << vs->codecpar->width << endl;
			cout << "height:" << vs->codecpar->height << endl;
		}
	}

	//编码器上下文
	const char* out_url = "outaa.mp4";
	AVFormatContext* ec = nullptr;
	re = avformat_alloc_output_context2(&ec, NULL, NULL, out_url);
	CERR(re);
	auto mvs = avformat_new_stream(ec, NULL);
	auto mas = avformat_new_stream(ec, NULL);

	//打开输出io
	re = avio_open(&ec->pb, out_url, AVIO_FLAG_WRITE);
	CERR(re);

	//设置音频流和视频流
	if (vs)
	{
		mvs->time_base = vs->time_base;
		avcodec_parameters_copy(mvs->codecpar, vs->codecpar);
	}
	if (as)
	{
		mas->time_base = as->time_base;
		avcodec_parameters_copy(mas->codecpar, as->codecpar);
	}

	double start_sec = 10.0;
	double duration_sec = 10.0;
	double end_sec = start_sec + duration_sec;
	long long start_pts = 0;
	long long end_pts = 0;

	double auido_start_sec = 10.0;
	double auido_duration_sec = 10.0;
	double auido_end_sec = auido_start_sec + auido_duration_sec;
	long long auido_start_pts = 0;
	long long auido_end_pts = 0;

	if (vs && vs->time_base.num > 0)
	{
		double t = (double)vs->time_base.den / (double)vs->time_base.num;
		start_pts = start_sec * t;
		end_pts = end_sec * t;
	}

	if (as && as->time_base.num > 0)
	{
		double auido_t = (double)as->time_base.den / (double)as->time_base.num;
		auido_start_pts = auido_start_sec * auido_t;
		auido_end_pts = auido_end_sec * auido_t;
	}

	// 移动到关键帧
	if (vs)
	{
		av_seek_frame(ic, vs->index, start_pts, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);
		CERR(re);
	}
	
	if (as)
	{
		av_seek_frame(ic, as->index, auido_start_pts, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);
		CERR(re);
	}


	re = avformat_write_header(ec, NULL);
	CERR(re);
	//打印封装信息
	av_dump_format(ec, 0, out_url,
		1	//0表示上下文是输入，1表示上下文是输出
	);




	AVPacket pkt;
	for (;;)
	{

		re = av_read_frame(ic, &pkt);
		

		if (re != 0)
		{
			break;
		}

		AVStream* in_stream = ic->streams[pkt.stream_index];
		AVStream* out_stream = nullptr;

		//this_thread::sleep_for(100ms);
		if (vs && pkt.stream_index == vs->index)
		{
			cout << "视频 ";
			out_stream = ec->streams[0];

			//只截取10秒
			if (pkt.pts>end_pts)
			{
				av_packet_unref(&pkt);
				break;
			}
			pkt.pts = av_rescale_q_rnd(pkt.pts - start_pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

			pkt.dts = av_rescale_q_rnd(pkt.dts - start_pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

		}
		else if (as && pkt.stream_index == as->index)
		{
			cout << "音频 ";
			out_stream = ec->streams[1];

			//只截取10秒
			if (pkt.pts > auido_end_pts)
			{
				av_packet_unref(&pkt);
				break;
			}

			// 只处理视频
			//av_packet_unref(&pkt);
			//continue;

			pkt.pts = av_rescale_q_rnd(pkt.pts - auido_start_pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

			pkt.dts = av_rescale_q_rnd(pkt.dts - auido_start_pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

		}
		
		//cout << "pkt.pts: " << pkt.pts << " " << "pkt.dts " << pkt.dts << " " << "pkt.size " << pkt.size << endl;
		
		

		pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);

		pkt.pos = -1;
		
		//写入音视频会清理pkt，所以下面不用再清理
		re = av_interleaved_write_frame(ec, &pkt);
		CERR(re);
		//av_packet_unref(&pkt);
	}
	//一定要写入结尾，否则视频播放不了
	re = av_write_trailer(ec);
	avformat_close_input(&ic);//需要成对出现
	avio_closep(&ec->pb);
	avformat_free_context(ec);
	ec = nullptr;//这一步需要做！
	return 0;
}