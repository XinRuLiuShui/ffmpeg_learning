#include <iostream>
using namespace std;

#include "xtools.h"
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
#include "xvideo_view.h"

#include "xdemux_task.h"
#include "xdecode_task.h"
#include "xmux_task.h"

//预处理指令导入库
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")//使用av_strerror需要添加这个库
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swscale.lib")//图像缩放库


int main(int argc, char* argv[])
{

	//string in_file = "rtsp://127.0.0.1:8554/test";
	string in_file = "rtsp://stream.strba.sk:1935/strba/VYHLAD_JAZERO.stream";
	XDemuxTask demux_task;
	
	cout << "正在连接RTSP流: " << in_file << endl;
	cout << "如果流还未启动，将每隔100ms重试..." << endl;
	
	for (;;)
	{
		if (demux_task.Open(in_file))
		{
			cout << "成功连接到RTSP流!" << endl;
			break;
		}
		cout << "连接失败，正在重试..." << endl;
		MSleep(100);
	}

	AVCodecParameters* video_para = nullptr;
	AVCodecParameters* audio_para = nullptr;
	AVRational* video_time_base = nullptr;
	AVRational* audio_time_base = nullptr;

	auto vpara = demux_task.CopyVideoPara();
	auto apara = demux_task.CopyAudioPara();

	if (vpara)
	{
		video_para = vpara->para;
		video_time_base = vpara->time_base;
	}
	if (apara)
	{
		audio_para = apara->para;
		audio_time_base = apara->time_base;
	}

	string url = "29_rtsp_remux_chain_out.mp4";

	XMuxTask mux_task;
	if (!mux_task.Open(url.c_str(), video_para, video_time_base, audio_para, audio_time_base))
	{
		LOGERROR("mux_task.Open failed!");
	}
	





	auto view = XVideoView::Creat(XVideoView::SDL);
	view->Init(vpara->para);

	XDecodeTask decode_task;
	if (!decode_task.Open(vpara->para))
	{
		LOGERROR("decode_task.Open failed!");
	}
	else
	{
		LOGINFO("decode_task.Open success");
		//demux_task.set_next(&decode_task);
		demux_task.set_next(&mux_task);
		demux_task.Start();
		decode_task.Start();
	}

	mux_task.Start();
	MSleep(5000);
	mux_task.Stop();

	for (;;)
	{
		auto f = decode_task.GetFrame();
		view->DrawFrame(f);
		XFreeFrame(&f);
	}
	

	cout << "开始读取数据..." << endl;
	getchar();
	return 0;
}