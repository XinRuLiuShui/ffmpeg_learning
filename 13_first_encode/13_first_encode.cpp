#include <iostream>
#include <fstream>

extern "C" {
	#include <libavcodec/avcodec.h>
}

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")//使用av_strerror需要添加这个库

using namespace std;

int main(int argc, char* argv[])
{
	string filename = "400_300_25";
	AVCodecID codec_id = AV_CODEC_ID_H264;
	if (argc > 1)
	{
		string codec = argv[1];
		if (codec == "h265" || codec == "hevc")
		{
			codec_id = AV_CODEC_ID_H265;
		}
	}
	if (codec_id == AV_CODEC_ID_H264)
	{
		filename += ".h264";
	}
	else if (codec_id == AV_CODEC_ID_H265)
	{
		filename += ".h265";
	}
	ofstream ofs;
	ofs.open(filename,ios::binary);

	//1.找到编码器
	auto codec = avcodec_find_encoder(codec_id);
	if (!codec)
	{
		cerr << "codec no found!" << endl;
		return -1;
	}
	//2.编码上下文
	auto c = avcodec_alloc_context3(codec);
	if (!c)
	{
		cerr << "avcodec_alloc_context3 failed!" << endl;
		return -1;
	}
	//3.设定上下文参数 
	c->width = 400;
	c->height = 300;

	//帧时间戳的时间单位 pts*time_base=播放时间（秒）
	c->time_base = { 1,25 };//分子和分母

	c->pix_fmt =AV_PIX_FMT_YUV420P;//元数据像是格式要与编码算法所支持的一致
	c->thread_count = 6;//编码线程数

	//打开编码上下文。
	int re = avcodec_open2(c, codec, NULL);
	if (re != 0)
	{
		char buf[1024];
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << buf << endl;
		return -1;
	}
	cout << "avcodec_open2 success" << endl;

	AVFrame* frame = av_frame_alloc();
	frame->width = c->width;
	frame->height = c->height;
	frame->format = c->pix_fmt;

	//生成AVframe空间,默认对齐参数,有可能会报错 
	re = av_frame_get_buffer(frame, 0);
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf));
		cout << buf << endl;
		//生成空间失败同样要释放 
		av_frame_free(&frame);
		return -1;
	}


	auto pkt = av_packet_alloc();
	//生成并压缩250帧数据
	for (int i = 0; i < 250; i++)
	{
		for (int y = 0; y < c->height; y++)
		{
			for (int x = 0; x < c->width; x++)
			{
				frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
			}
		}

		for (int y = 0; y < c->height / 2; y++)
		{
			for (int x = 0; x < c->width / 2; x++)
			{
				frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
				frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
			}
		}
		frame->pts = i;//显示的时间
		//将原始数据发送给线程进行压缩
		re = avcodec_send_frame(c, frame);
		if (re != 0)
		{
			break;
		}

		while (re >= 0)
		{
			//调用之后会立刻返回，但不代表已经压缩完成，需要重复询问
			//每次调用会重新生成pkt的空间，所以调用完之后需要释放
			re = avcodec_receive_packet(c, pkt);
			if (re == AVERROR(EAGAIN) || re == AVERROR_EOF)
			{
				break;
			}
			if (re != 0)
			{
				char buf[1024] = { 0 };
				av_strerror(re, buf, sizeof(buf));
				cout << buf << endl;
				break;
			}
			cout << pkt->size << " " << flush;
			ofs.write((char*)pkt->data, pkt->size);
			av_packet_unref(pkt);
		}
		
	}
	
	ofs.close();
	av_frame_free(&frame);

	//释放上下文
	avcodec_free_context(&c);

	cout << avcodec_configuration() << endl;
}