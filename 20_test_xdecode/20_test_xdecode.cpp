#include <iostream>
#include <fstream>
#include <string>
#include "xvideo_view.h"
extern "C" {
	#include <libavcodec/avcodec.h>
}

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")//使用av_strerror需要添加这个库
#include "xdecode.h"
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

	XDecode de;
	auto c = de.Creat(codec_id, false);

	de.set_c(c);

	//硬件加速方式	DXVA2
	auto hw_type = AV_HWDEVICE_TYPE_DXVA2;
	// 初始化硬件加速
	if (!de.InitHWDevice(hw_type))
	{
		std::cerr << "HW init failed, fallback to CPU" << std::endl;
	}


	de.Open();



	


	//c->thread_count = 16;


	////初始化硬件加速上下文
	//AVBufferRef* hw_ctx = nullptr;
	//av_hwdevice_ctx_create(&hw_ctx, hw_type, NULL, NULL, 0);
	////c->hw_device_ctx = av_buffer_ref(hw_ctx);


	auto pkt = av_packet_alloc();

	//分割上下文
	auto parser = av_parser_init(codec_id);

	while (!ifs.eof())
	{

		ifs.read((char*)inbuf, sizeof(inbuf));
		int data_size = ifs.gcount();
		if (data_size <= 0)
		{
			break;
		}

		//if (ifs.eof())
		//{
		//	ifs.clear();
		//	ifs.seekg(0, ios::beg);
		//}

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

				if (!de.Send(pkt))
				{
					break;
				}

				while (de.Recv(frame))
				{
					
					
					//ret = avcodec_receive_frame(c, frame);
					//if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
					//	break;
					//else if (ret < 0)
					//	break;   // 或报错处理

					auto pframe = frame;
					//if (c->hw_device_ctx)
					//{
					//	pframe = hw_frame;
					//	//显存->内存
					//	av_hwframe_transfer_data(hw_frame, frame, 0);
					//}

					//cout << "(" << frame->format << " " << flush;
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



	cout << "start flush" << endl;
	// 1.告诉解码器：没有数据了，开始 flush
	//int ret = avcodec_send_packet(c, NULL);
	//if (ret < 0) {
	//	// flush 失败（一般不会）
	//	//break;
	//	//return;
	//}

	// 2.不断取缓存帧
	//while (true)
	//{
	//	ret = avcodec_receive_frame(c, frame);

	//	if (ret == 0)
	//	{
	//		// 成功拿到一帧，处理 frame
	//		cout << "(" << frame->format << ")" << " " << flush;
	//	}
	//	else if (ret == AVERROR(EAGAIN))
	//	{
	//		// 理论上 flush 阶段很少见，说明还需要再 send(NULL) 一次
	//		break;
	//	}
	//	else if (ret == AVERROR_EOF)
	//	{
	//		// 真正结束，没有帧了
	//		break;
	//	}
	//	else
	//	{
	//		// 其他错误
	//		break;
	//	}
	//}

	auto frames = de.End();
	for (auto get_frame : frames)
	{
		count++;
		view->DrawFrame(get_frame);
		av_frame_unref(get_frame);
	}

	av_frame_free(&frame);
	av_packet_free(&pkt);

	//释放上下文
	avcodec_free_context(&c);

	av_parser_close(parser);

	cout << "(" << count << ")" << " " << endl;
	
}