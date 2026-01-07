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

private:
    Ui::XViewerClass ui;
    QMenu left_menu_;
};

