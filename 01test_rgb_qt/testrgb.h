#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_testrgb.h"

class TestRGB : public QMainWindow
{
    Q_OBJECT

public:
    TestRGB(QWidget *parent = nullptr);
    ~TestRGB();

    //重载绘制画面函数
    void paintEvent(QPaintEvent* ev) override;
private:
    Ui::TestRGBClass ui;
};

