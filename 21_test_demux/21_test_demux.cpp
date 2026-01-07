#include <iostream>
#include <thread>
using namespace std;
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

//预处理指令导入库
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")//使用av_strerror需要添加这个库
#pragma comment(lib,"avcodec.lib")

bool PrintErr(int re)
{
	if (re != 0)
	{
		char buf[1024];
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << buf << endl;
		return false;
	}
	return true;
}
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
		if (ic->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO)
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

	AVPacket pkt;
	for (;;)
	{
		re = av_read_frame(ic, &pkt);
		//this_thread::sleep_for(100ms);
		if (vs&&pkt.stream_index==vs->index)
		{
			cout << "视频 ";
		}
		else if (as && pkt.stream_index == as->index)
		{
			cout << "音频 ";
		}
		CERR(re);
		cout << "pkt.pts: " << pkt.pts << " " << "pkt.dts " << pkt.dts << " " << "pkt.size " << pkt.size << endl;
		av_packet_unref(&pkt);
	}
	avformat_close_input(&ic);//需要成对出现
	return 0;
}