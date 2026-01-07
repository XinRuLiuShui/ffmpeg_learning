#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_sdlqtrbg.h"

class SdlQtRBG : public QMainWindow
{
    Q_OBJECT

public:
    SdlQtRBG(QWidget *parent = nullptr);
    void timerEvent(QTimerEvent* ev) override;
    ~SdlQtRBG();

private:
    Ui::SdlQtRBGClass ui;
};

