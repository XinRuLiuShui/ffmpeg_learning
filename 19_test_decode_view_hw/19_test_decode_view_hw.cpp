#include <iostream>
#include <fstream>
#include <string>
#include "xvideo_view.h"
extern "C" {
	#include <libavcodec/avcodec.h>
}

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")//使用av_strerror需要添加这个库

using namespace std;

int main(int argc, char* argv[])
{
	string filename = "test.h264";
	ifstream ifs(filename, ios::binary);
	if (!ifs)
	{
		return -1;
	}
	unsigned char inbuf[4096] = { 0 };
	AVCodecID codec_id = AV_CODEC_ID_H264;

	AVFrame* frame = av_frame_alloc();
	AVFrame* hw_frame = av_frame_alloc();

	auto begin = NowMs();
	int count = 0;


	auto view = XVideoView::Creat(XVideoView::SDL);
	bool win_is_init = false;

	//1.找到解码器
	auto codec = avcodec_find_decoder(codec_id);
	if (!codec)
	{
		cerr << "codec no found!" << endl;
		return -1;
	}

	//硬件加速方式	DXVA2
	auto hw_type = AV_HWDEVICE_TYPE_DXVA2;

	//打印所有硬件加速方式
	for (int i = 0;; ++i)
	{
		const AVCodecHWConfig* config = avcodec_get_hw_config(codec, i);
		if (!config)
			break;   // 没有更多配置，退出

		const char* hw_type = av_hwdevice_get_type_name(config->device_type);
		std::cout << "  " << i
			<< ": " << (hw_type ? hw_type : "unknown")
			<< "  method=" << config->methods
			<< std::endl;
	}


	

	//2.解码上下文
	auto c = avcodec_alloc_context3(codec);
	if (!c)
	{
		cerr << "avcodec_alloc_context3 failed!" << endl;
		return -1;
	}

	c->thread_count = 16;


	//初始化硬件加速上下文
	AVBufferRef* hw_ctx = nullptr;
	av_hwdevice_ctx_create(&hw_ctx, hw_type, NULL, NULL, 0);
	//c->hw_device_ctx = av_buffer_ref(hw_ctx);

	//打开解码上下文
	int re = avcodec_open2(c, NULL, NULL);
	if (re != 0)
	{
		char buf[1024];
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << buf << endl;
		return -1;
	}
	cout << "avcodec_open2 success" << endl;

	auto pkt = av_packet_alloc();

	//分割上下文
	auto parser = av_parser_init(codec_id);

	//ifs.seekg(0, ios::end);    //移到文件结尾
	//int file_size = ifs.tellg();   //文件指针的位置
	//ifs.seekg(0, ios::beg);    //移到文件开关
	while (!ifs.eof())
	{
		//if (ifs.tellg() == file_size)
		//{
		//	ifs.seekg(0, ios::beg);
		//}

		ifs.read((char*)inbuf, sizeof(inbuf));
		int data_size = ifs.gcount();
		if (data_size <= 0)
		{
			break;
		}

		if (ifs.eof())
		{
			ifs.clear();
			ifs.seekg(0, ios::beg);
		}

		auto data = inbuf;

		while (data_size > 0)//可能一次 read 里有多帧
		{
			//通过0001截断 输出到av packet中
			int ret = av_parser_parse2(parser, c,
				&pkt->data, &pkt->size,
				data, data_size,
				AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0
			);
			data += ret;
			data_size -= ret;//已处理

			if (data_size > 0)
			{
				//cout << pkt->size << " " << flush;
				//发送av packet到解码线程
				ret = avcodec_send_packet(c, pkt);
				if (ret < 0)
				{
					break;
				}
				//获取多帧数据
				//while (ret > 0)
				//{
				//	//每次会调用av_frame_unref函数，frame会被重新设置
				//	ret = avcodec_receive_frame(c, frame);
				//	if (ret < 0)
				//	{
				//		break;
				//	}
				//	cout << frame->format << " " << flush;
				//}

				while (true) 
				{
					ret = avcodec_receive_frame(c, frame);
					if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
						break;
					else if (ret < 0)
						break;   // 或报错处理

					auto pframe = frame;
					if (c->hw_device_ctx)
					{
						pframe = hw_frame;
						//显存->内存
						av_hwframe_transfer_data(hw_frame, frame, 0);
					}

					cout << "(" << frame->format << " " << flush;
					// 成功得到一帧，处理 frame
					//初始化窗口
					if (!win_is_init)
					{
						view->Init(pframe->width, pframe->height, (XVideoView::Format)pframe->format);
						win_is_init = true;
					}
					view->DrawFrame(pframe);

					count++;
					auto cur = NowMs();
					if (cur - begin >= 1000)
					{
						cout << "\nfps = " << count << endl;
						count = 0;
						begin = cur;
					}
				}
			}
			//cout << pkt->size << " ";
		}
		
	}

	//int ret= avcodec_send_packet(c, NULL);
	//while (ret > 0)
	//{
	//	//每次会调用av_frame_unref函数，frame会被重新设置
	//	ret = avcodec_receive_frame(c, frame);
	//	if (ret < 0)
	//	{
	//		break;
	//	}
	//	cout << frame->format << " " << flush;
	//}

	cout << "start flush" << endl;
	// 1.告诉解码器：没有数据了，开始 flush
	int ret = avcodec_send_packet(c, NULL);
	if (ret < 0) {
		// flush 失败（一般不会）
		//break;
		//return;
	}

	// 2.不断取缓存帧
	while (true)
	{
		ret = avcodec_receive_frame(c, frame);

		if (ret == 0)
		{
			// 成功拿到一帧，处理 frame
			cout << "(" << frame->format << ")" << " " << flush;
		}
		else if (ret == AVERROR(EAGAIN))
		{
			// 理论上 flush 阶段很少见，说明还需要再 send(NULL) 一次
			break;
		}
		else if (ret == AVERROR_EOF)
		{
			// 真正结束，没有帧了
			break;
		}
		else
		{
			// 其他错误
			break;
		}
	}

	av_frame_free(&frame);
	av_packet_free(&pkt);

	//释放上下文
	avcodec_free_context(&c);

	av_parser_close(parser);
	
}