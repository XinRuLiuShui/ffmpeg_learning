#include "sdlqtrbg.h"
#include "xvideo_view.h"
//#include "SDL.h"
#include <QLabel>          // 加这一行
#include <fstream>
#include <iostream>
#include <QMessageBox>
#include <qspinbox.h>
#include <qfiledialog.h>
#include <thread>
#include <sstream>
#include <vector>

using namespace std;

extern "C" {
#include <libavcodec/avcodec.h>
}

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")

using namespace std;

//#pragma comment(lib,"SDL2.lib")
//#undef main

//static XVideoView* view = nullptr;
//static XVideoView* view1 = nullptr;
//static XVideoView* view2 = nullptr;
//static int sdl_render_width = 0;
//static int sdl_render_height = 0;
//static int pix_size = 2;
//static unsigned char* yuv = NULL;
//static unsigned char* Mirror_yuv = NULL;
//static unsigned char* Mirror_y = NULL;
//static unsigned char* Mirror_u = NULL;
//static unsigned char* Mirror_v = NULL;
//
//static QLabel* view_fps = nullptr;
//static QSpinBox* set_fps = nullptr;
//int fps = 25;
//
//static long long file_size = 0;
//
//static AVFrame* frame = nullptr;
//
//static ifstream yuv_file;


vector<XVideoView*> views;

SdlQtRBG::SdlQtRBG(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    //绑定渲染信号槽
    connect(this, SIGNAL(ViewS()), this, SLOT(View()));

    views.push_back(XVideoView::Creat(XVideoView::SDL));
    views.push_back(XVideoView::Creat(XVideoView::SDL));
    views[0]->set_win_id((void*)ui.video1->winId());
    views[1]->set_win_id((void*)ui.video2->winId());
    
    th_ = std::thread(&SdlQtRBG::Main, this);
}

void SdlQtRBG::timerEvent(QTimerEvent* ev)
{
    

}

void SdlQtRBG::resizeEvent(QResizeEvent* ev)
{
    //ui.label->resize(size());
    //ui.label->move(0, 0);   //加这行对齐左上角
    //view->SetScale(ui.label->width(), ui.label->height());
}

void SdlQtRBG::Main()
{
    while (!isExit_)
    {
        ViewS();
        MSleep(1000 / 100);
    }
}

void SdlQtRBG::View()
{
    //存放上次渲染的时间戳 
    static int last_pts[32] = { 0 };
    static int set_fps_arry[32] = { 0 };
    set_fps_arry[0] = ui.set_fps1->value();
    set_fps_arry[1] = ui.set_fps2->value();

    for (int i = 0; i < views.size(); i++)
    {
        if (set_fps_arry[i] <= 0)
        {
            continue;
        }
        int ms = 1000 / set_fps_arry[i];

        if (NowMs() - last_pts[i] >= ms)
        {
            last_pts[i] = NowMs();
        }
        else {
            continue;
        }

        auto frame = views[i]->Read();
        if (!frame)
        {
            continue;
        }
        views[i]->DrawFrame(frame);
        stringstream ss;
        ss << "fps:" << views[i]->render_fps();
        //只能在槽函数里调用
        if (i == 0)
        {
            ui.fps1->setText(ss.str().c_str());
        }
        else
        {
            ui.fps2->setText(ss.str().c_str());
        }
        
    }
    //auto frame = views[0]->Read();
    //views[0]->DrawFrame(frame);
}

SdlQtRBG::~SdlQtRBG()
{
    //等待渲染线程退出
    isExit_ = true;
    th_.join();
}

void SdlQtRBG::Open(int i)
{
    QFileDialog fd;
    auto filename = fd.getOpenFileName();
    if (filename.isEmpty())
    {
        return;
    }
    cout << filename.toLocal8Bit().data() << endl;
    if (!views[i]->Open(filename.toStdString()))
    {
        return;
    }

    int w = 0;
    int h = 0;
    QString pix = 0;
    XVideoView::Format fmt = XVideoView::YUV420P;

    if (i == 0)
    {
        w = ui.width1->value();
        h = ui.height1->value();
        pix = ui.pix1->currentText();
    }
    else if (i == 1)
    {
        w = ui.width2->value();
        h = ui.height2->value();
        pix = ui.pix2->currentText();
    }
    
    if (pix == "YUV420P")
    {
        //fmt = XVideoView::YUV420P;
    }
    else if (pix == "RGBA")
    {
        fmt = XVideoView::RGBA;
    }
    else if (pix == "ARGB")
    {
        fmt = XVideoView::ARGB;
    }
    else if (pix == "BGRA")
    {
        fmt = XVideoView::BGRA;
    }
    else if (pix == "RGB24")
    {
        fmt = XVideoView::RGB24;
    }
    views[i]->Init(w, h, fmt);
}

void SdlQtRBG::Open1()
{
    Open(0);
}

void SdlQtRBG::Open2()
{
    Open(1);
}

