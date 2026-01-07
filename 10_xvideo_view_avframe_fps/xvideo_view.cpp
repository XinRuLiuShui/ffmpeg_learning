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
		return Draw(frame->data[0], frame->linesize[0]);
		break;
	default:
		break;
	}
	return false;
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
