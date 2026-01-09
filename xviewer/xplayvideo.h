#pragma once

#include <QMainWindow>
#include "ui_xplayvideo.h"
#include "xdemux_task.h"
#include "xdecode_task.h"
#include "xvideo_view.h"

class XPlayVideo : public QMainWindow
{
	Q_OBJECT

public:
	XPlayVideo(QWidget *parent = nullptr);
	~XPlayVideo();
	bool Open(const char* url);
	void timerEvent(QTimerEvent* ev) override;
	void closeEvent(QCloseEvent* ev) override;

	void Close();
private:
	Ui::XPlayVideoClass ui;
	XDecodeTask decode_task_;
	XDemuxTask demux_task_;
	XVideoView* view_ = nullptr;
};

