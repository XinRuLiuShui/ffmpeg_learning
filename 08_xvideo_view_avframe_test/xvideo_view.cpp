#include "xvideo_view.h"
#include "xsdl.h"
extern "C" {
#include <libavcodec/avcodec.h>
}
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
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
