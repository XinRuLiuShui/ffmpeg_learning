#pragma once
#include <QtWidgets/QWidget>

class XDecodeTask;
class XDemuxTask;
class XVideoView;

class XCameraWidget :
    public QWidget
{
    //必须要加，否则无法使用槽函数
    Q_OBJECT
public:
    XCameraWidget(QWidget* p = nullptr);

    //拖拽进入
    void dragEnterEvent(QDragEnterEvent* ev) override;
    //拖拽松开
    void dropEvent(QDropEvent* ev) override;

    //渲染重载
    void paintEvent(QPaintEvent* ev);

    //打开播放视频
    bool Open(const char* url);

    void Draw();

    //清理资源
    ~XCameraWidget();
private:
    XDecodeTask* decode_task_ = nullptr;
    XDemuxTask* demux_task_ = nullptr;
    XVideoView* view_ = nullptr;
};

