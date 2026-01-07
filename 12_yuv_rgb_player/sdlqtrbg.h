#pragma once

#include <QtWidgets/QMainWindow>
#include <thread>
#include "ui_sdlqtrbg.h"

class SdlQtRBG : public QMainWindow
{
    Q_OBJECT

public:
    SdlQtRBG(QWidget *parent = nullptr);
    void timerEvent(QTimerEvent* ev) override;
    void resizeEvent(QResizeEvent* ev) override;
    //线程函数，用于刷新视频
    void Main();

    ~SdlQtRBG();

private:
    std::thread th_;
    bool isExit_ = false;
    Ui::SdlQtRBGClass ui;

signals:
    void ViewS();   //信号函数，将任务放入列表
public slots:
    void View();    //显示的槽函数
    void Open(int i);
    void Open1();
    void Open2();
};

