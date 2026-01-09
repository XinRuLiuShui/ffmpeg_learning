#pragma once

//#include <QtWidgets/QWidget>
#include "ui_xviewer.h"
#include <QMenu>

class XViewer : public QWidget
{
    Q_OBJECT

public:
    XViewer(QWidget *parent = nullptr);
    ~XViewer();

    //鼠标事件用于拖动窗口
    void mouseMoveEvent(QMouseEvent* ev) override;
    void mousePressEvent(QMouseEvent* ev) override;
    void mouseReleaseEvent(QMouseEvent* ev) override;
    //显示预览相机窗口
    void View(int count);
    //刷新左侧相机列表
    void ReflashCams();

    void timerEvent(QTimerEvent* ev) override;
public slots:
    
    void MaxWindow();
    void NormalWindow();

    //窗口大小发生变化
    void resizeEvent(QResizeEvent* ev) override;
    //右键菜单
    void contextMenuEvent(QContextMenuEvent* event) override;

    void View1();
    void View4();
    void View9();
    void View16();

    void SetCam(int index);
    void AddCam();
    void SetCam();
    void DelCam();

    void StartRecord(); //所有摄像机开始录制
    void StopRecord();  //所有摄像机停止录制

    void Preview();     //预览窗口
    void Playback();    //回放窗口

    void SelectCamera(QModelIndex); //选择摄像机
    void SelectDate(QDate); //选择日期
    void PlayVideo(QModelIndex);    //选择视频
private:
    Ui::XViewerClass ui;
    QMenu left_menu_;
};

