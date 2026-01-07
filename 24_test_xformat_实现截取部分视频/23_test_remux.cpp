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

//int main(int argc, char* argv[])
//{
//	//打开媒体文件
//	const char* url = "v1080.mp4";
//
//	AVFormatContext* ic = nullptr;
//	auto re = avformat_open_input(&ic, url,
//		NULL,	//封装器格式 为空自动根据后缀名或者文件头检测
//		NULL	//参数设置 rtsp需要设置
//	);
//	CERR(re);
//
//	//获取媒体信息,无头部文件
//	re = avformat_find_stream_info(ic, NULL);
//	CERR(re);
//
//	//打印封装信息
//	av_dump_format(ic, 0, url,
//		0	//0表示上下文是输入，1表示上下文是输出
//	);
//
//	AVStream* as = nullptr;
//	AVStream* vs = nullptr;
//	for (int i = 0; i < ic->nb_streams; i++)
//	{
//		if (ic->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO)
//		{
//			as = ic->streams[i];
//			cout << "===========音频===========" << endl;
//			cout << "sample reat:" << as->codecpar->sample_rate << endl;
//		}
//		else if (ic->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
//		{
//			vs = ic->streams[i];
//			cout << "===========视频===========" << endl;
//			cout << "widht:" << vs->codecpar->width << endl;
//			cout << "height:" << vs->codecpar->height << endl;
//			cout << "format:" << vs->codecpar->format << endl;
//		}
//	}
//
//	/////////////////////////////////////////////////////////////////////////
//	//解封装
//	/////////////////////////////////////////
//	
//	AVFormatContext* format_context = avformat_alloc_context();
//	int ret = avformat_open_input(&format_context, url, NULL, NULL);
//	CERR(ret);
//
//	ret = avformat_find_stream_info(format_context, NULL);
//	CERR(ret);
//	cout << "---------------------" << endl;
//	// url: 调用 avformat_open_input 读取到的媒体文件的路径/名字
//	cout << "media name:" << format_context->url << endl;
//	// nb_streams: 媒体流数量
//	cout << "stream number:" << format_context->nb_streams << endl;
//	// bit_rate: 媒体文件的码率，单位为 bps
//	cout << "media average ratio:"
//		<< (format_context->bit_rate / 1024)   // 转成 kbps
//		<< "kbps" << endl;
//	// 时长换算
//	int total_seconds = static_cast<int>(format_context->duration / AV_TIME_BASE);
//	cout << "total duration: " << total_seconds;
//	//vs->codecpar->codec_id;
//
//	auto view = XVideoView::Creat(XVideoView::SDL);
//	bool win_is_init = false;
//
//	//注意是codec_id!
//	AVCodecID codec_id = vs->codecpar->codec_id;
//	XDecode de;
//	auto c = de.Creat(codec_id, false);
//
//	//一定要加这行代码!
//	avcodec_parameters_to_context(c, vs->codecpar);
//	//把“流里的参数（AVStream->codecpar）复制到解码器上下文（AVCodecContext）里”，
//	//让解码器真正知道视频是啥格式、分辨率多少、用什么 profile、SPS / PPS 在哪。
//	//加上它后，解码器才能正确解析 MP4 里的 H.264 数据，所以就不再报错了。
//	de.set_c(c);
//	de.Open();
//
//	AVPacket* pkt = av_packet_alloc();
//	AVFrame* frame = av_frame_alloc();
//	for (;;)
//	{
//		re = av_read_frame(ic, pkt);
//		//this_thread::sleep_for(100ms);
//		if (vs&&pkt->stream_index==vs->index)
//		{
//			cout << "视频 ";
//			cout << "video pts: " << pkt->pts << endl;
//			cout << "video dts: " << pkt->dts << endl;
//			cout << "video size: " << pkt->size << endl;
//			cout << "video pos: " << pkt->pos << endl;
//			cout << "video duration: "
//				<< pkt->duration * av_q2d(format_context->streams[vs->index]->time_base)
//				<< endl;
//
//
//			if (!de.Send(pkt))
//			{
//				break;
//			}
//
//			while (de.Recv(frame, false))
//			{
//
//				//初始化窗口
//				if (!win_is_init)
//				{
//					view->Init(frame->width, frame->height, (XVideoView::Format)frame->format);
//					win_is_init = true;
//				}
//				view->DrawFrame(frame);
//				
//			}
//
//		}
//		else if (as && pkt->stream_index == as->index)
//		{
//			//cout << "音频 ";
//		}
//		//CERR(re);
//		//cout << "pkt.pts: " << pkt.pts << " " << "pkt.dts " << pkt.dts << " " << "pkt.size " << pkt.size << endl;
//		
//		//写在 for 外面，会只释放最后一个包，属于 内存泄漏
//		av_packet_unref(pkt);
//	}
//
//	av_frame_free(&frame);
//	avformat_close_input(&ic);//需要成对出现
//	avformat_free_context(format_context);
//	return 0;
//}