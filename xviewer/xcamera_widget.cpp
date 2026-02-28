#include "xcamera_widget.h"
#include "xdemux_task.h"
#include "xdecode_task.h"
#include "xvideo_view.h"
#include "xcamera_config.h"

#include <QStyleOption>
#include <QPainter>
#include <QDragEnterEvent>
#include <QDebug>
#include <QListWidget>
XCameraWidget::XCameraWidget(QWidget* p):QWidget(p)
{
	//允许接收拖拽
	this->setAcceptDrops(true);
}

void XCameraWidget::dragEnterEvent(QDragEnterEvent* ev)
{

	ev->acceptProposedAction();

}

void XCameraWidget::dropEvent(QDropEvent* ev)
{
	//qDebug() << ev->source()->objectName();
	auto wid = (QListWidget*)ev->source();
	qDebug() << wid->currentRow();
	auto cam = XCameraConfig::Instance()->GetCam(wid->currentRow());
	Open(cam.url);
}

//让 Qt 的 QSS（样式表）能正确作用到你的自定义 QWidget 上
void XCameraWidget::paintEvent(QPaintEvent* ev)
{
	QStyleOption opt;
	opt.init(this);
	QPainter painter(this);
	//让 Qt Style 系统，按当前 QSS 规则，绘制这个 QWidget 自身
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

bool XCameraWidget::Open(const char* url)
{
	if (demux_task_)
	{
		demux_task_->Stop();
		delete demux_task_;
		demux_task_ = nullptr;
	}
	if (decode_task_)
	{
		decode_task_->Stop();
		delete decode_task_;
		decode_task_ = nullptr;
	}
	if (view_)
	{
		view_->Close();
		delete view_;
		view_ = nullptr;
	}
	//创建解封装线程
	demux_task_ = new XDemuxTask();
	if (!demux_task_->Open(url))
	{
		return false;
	}

	//创建解码线程
	decode_task_ = new XDecodeTask();
	auto para = demux_task_->CopyVideoPara();
	if (!decode_task_->Open(para->para))
	{
		return false;
	}
	demux_task_->set_next(decode_task_);
	
	decode_task_->set_video_stream_index(demux_task_->video_index());
	decode_task_->set_stream_index(demux_task_->video_index());
	//创建渲染器
	view_ = XVideoView::Creat(XVideoView::SDL);
	view_->set_win_id((void*)winId());
	view_->Init(para->para);

	//启动线程
	demux_task_->Start();
	decode_task_->Start();
	return true;
}

void XCameraWidget::Draw()
{
	if (!demux_task_ || !decode_task_ || !view_)
	{
		return;
	}
	auto frame = decode_task_->GetFrame();
	if (!frame)
	{
		return;
	}
	view_->DrawFrame(frame);
	XFreeFrame(&frame);
}

XCameraWidget::~XCameraWidget()
{
	if (demux_task_)
	{
		demux_task_->Stop();
		delete demux_task_;
		demux_task_ = nullptr;

	}
	if (decode_task_)
	{
		decode_task_->Stop();
		delete decode_task_;
		decode_task_ = nullptr;
	}
	if (view_)
	{
		view_->Close();
		delete view_;
		view_ = nullptr;
	}
}
