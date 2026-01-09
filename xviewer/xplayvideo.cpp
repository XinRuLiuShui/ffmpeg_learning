#include "xplayvideo.h"

XPlayVideo::XPlayVideo(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	//启动定时器渲染视频
	startTimer(1);
}

XPlayVideo::~XPlayVideo()
{
	//demux_task_.Stop();
	//decode_task_.Stop();
	//view_->Close();
}

bool XPlayVideo::Open(const char* url)
{
	demux_task_.Stop();
	decode_task_.Stop();
	
	if (!demux_task_.Open(url))
	{
		return false;
	}
	auto para = demux_task_.CopyVideoPara();

	if (!para)
	{
		return false;
	}
	if (!decode_task_.Open(para->para))
	{
		return false;
	}
	demux_task_.set_next(&decode_task_);
	decode_task_.set_video_stream_index(demux_task_.video_index());
	//创建渲染器
	if (!view_)
	{
		view_ = XVideoView::Creat(XVideoView::SDL);
	}
	view_->set_win_id((void*)winId());
	view_->Init(para->para);

	demux_task_.set_syn_type(XSYN_VIDEO);

	//启动线程
	demux_task_.Start();
	decode_task_.Start();

	

	return true;
}

void XPlayVideo::timerEvent(QTimerEvent* ev)
{
	if (!view_)
	{
		return;
	}
	auto frame = decode_task_.GetFrame();
	if (!frame)
	{
		return;
	}
	view_->DrawFrame(frame);
	XFreeFrame(&frame);
}

void XPlayVideo::closeEvent(QCloseEvent* ev)
{
	Close();
}

void XPlayVideo::Close()
{
	demux_task_.Stop();
	decode_task_.Stop();
	if (view_)
	{
		view_->Close();
		delete view_;
		view_ = nullptr;
	}
}

