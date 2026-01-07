#include "xvideo_view.h"
#include "xsdl.h"
#include <thread>
#include <ctime>
#include <iostream>
extern "C" {
#include <libavcodec/avcodec.h>
}
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")

using namespace std;

XVideoView* XVideoView::Creat(RenderType type = SDL)
{
	switch (type)
	{
	case XVideoView::SDL:
		return new XSDL();
		break;
	default:
		break;
	}
	return nullptr;
}

bool XVideoView::DrawFrame(AVFrame* frame)
{
	if (!frame || !frame->data[0])
	{
		return false;
	}
	count_++;
	if (beg_ms_ <= 0)
	{
		beg_ms_ = clock();
	}
	else if ((clock() - beg_ms_) / (CLOCKS_PER_SEC / 1000) >= 1000)
	{
		render_fps_ = count_;
		count_ = 0;
		beg_ms_ = clock();
	}

	switch (frame->format)
	{
	case AV_PIX_FMT_YUV420P:
		return Draw(
			frame->data[0], frame->linesize[0],  //Y
			frame->data[1], frame->linesize[1], //U
			frame->data[2], frame->linesize[2]	//V  (多了一个,会报错)
			);
		break;
	case AV_PIX_FMT_BGRA:
	case AV_PIX_FMT_ARGB:
	case AV_PIX_FMT_RGBA:
		return Draw(frame->data[0], frame->linesize[0]);
		break;
	case AV_PIX_FMT_RGB24:
		return Draw(frame->data[0], 0);
		break;
	default:
		break;
	}
	return false;
}

bool XVideoView::Open(std::string filepath)
{
	if (ifs_.is_open())
	{
		ifs_.close();
	}
	ifs_.open(filepath, ios::binary);

	ifs_.seekg(0, ios::end);    //移到文件结尾
	file_size_ = ifs_.tellg();   //文件指针的位置
	ifs_.seekg(0, ios::beg);    //移到文件开关

	return ifs_.is_open();
}

AVFrame* XVideoView::Read()
{
	if (width_ <= 0 || height_ <= 0 || !ifs_)
	{
		return nullptr;
	}

	//如果已经申请，但是参数发生变化的话，释放并重新分配 
	if (frame_)
	{
		if (frame_->width != width_
			|| frame_->height != height_
			|| frame_->format != fmt_
			)
		{
			av_frame_free(&frame_);
		}
	}

	//如果frame_不存在或者已经被释放 
	if (!frame_)
	{
		frame_ = av_frame_alloc();
		//图像参数
		frame_->width = width_;
		frame_->height = height_;
		frame_->format = fmt_;

		frame_->linesize[0] = width_ * 4;

		if (frame_->format == AV_PIX_FMT_YUV420P)
		{
			frame_->linesize[0] = width_;
			frame_->linesize[1] = width_ / 2;
			frame_->linesize[2] = width_ / 2;
		}
		else if (frame_->format == AV_PIX_FMT_RGB24)
		{
			frame_->linesize[0] = width_ * 3;
		}

		//生成AVframe空间,默认对齐参数,有可能会报错 
		int re = av_frame_get_buffer(frame_, 0);
		if (re != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(re, buf, sizeof(buf));
			cout << buf << endl;
			//生成空间失败同样要释放 
			av_frame_free(&frame_);
			return nullptr;
		}
	}
	
	if (!frame_)
	{
		return nullptr;
	}

	if (ifs_.tellg() == file_size_)
	{
		ifs_.seekg(0, ios::beg);
	}

	if (frame_->format == AV_PIX_FMT_YUV420P)
	{
		ifs_.read((char*)frame_->data[0], width_ * height_);
		ifs_.read((char*)frame_->data[1], width_ * height_ / 4);
		ifs_.read((char*)frame_->data[2], width_ * height_ / 4);
	}
	else if ( frame_->format == AV_PIX_FMT_BGRA
			|| frame_->format == AV_PIX_FMT_ARGB
			|| frame_->format == AV_PIX_FMT_RGBA )
	{
		ifs_.read((char*)frame_->data[0], width_ * height_ * 4);
	}
	else if (frame_->format == AV_PIX_FMT_RGB24)
	{
		ifs_.read((char*)frame_->data[0], width_ * height_ * 3);
	}

	if (ifs_.gcount() == 0)
	{
		return nullptr;
	}
	return frame_;
}

void MSleep(unsigned int ms)
{
	auto beg = clock();
	for (int i = 0; i < ms; i++)
	{
		this_thread::sleep_for(1ms);
		if ((clock() - beg) / (CLOCKS_PER_SEC / 1000) >= ms)
		{
			break;
		}
	}
}

long long NowMs()
{
	return clock() / (CLOCKS_PER_SEC / 1000);
}
