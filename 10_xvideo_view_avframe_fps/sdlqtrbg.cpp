#include "sdlqtrbg.h"
#include "xvideo_view.h"
//#include "SDL.h"
#include <QLabel>          // 加这一行
#include <fstream>
#include <iostream>
#include <QMessageBox>
#include <qspinbox.h>
#include <thread>
#include <sstream>
using namespace std;

extern "C" {
#include <libavcodec/avcodec.h>
}

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")

using namespace std;

//#pragma comment(lib,"SDL2.lib")
//#undef main

static XVideoView* view = nullptr;
static XVideoView* view1 = nullptr;
static XVideoView* view2 = nullptr;
static int sdl_render_width = 0;
static int sdl_render_height = 0;
static int pix_size = 2;
static unsigned char* yuv = NULL;
static unsigned char* Mirror_yuv = NULL;
static unsigned char* Mirror_y = NULL;
static unsigned char* Mirror_u = NULL;
static unsigned char* Mirror_v = NULL;

static QLabel* view_fps = nullptr;
static QSpinBox* set_fps = nullptr;
int fps = 25;

static long long file_size = 0;

static AVFrame* frame = nullptr;

static ifstream yuv_file;

SdlQtRBG::SdlQtRBG(QWidget *parent)
    : QMainWindow(parent)
{
    yuv_file.open("400_300_25.yuv",ios::binary);
    if (!yuv_file)
    {
        QMessageBox::information(this, "", "open yuv_file failed!");
        return;
    }
    yuv_file.seekg(0, ios::end);    //移到文件结尾
    file_size = yuv_file.tellg();   //文件指针的位置
    yuv_file.seekg(0, ios::beg);    //移到文件开关

    ui.setupUi(this);


    //绑定渲染信号槽
    connect(this, SIGNAL(ViewS()), this, SLOT(View()));


    auto label = new QLabel(this);
    //label->setText("100fps");
    view_fps = new QLabel(this);
    view_fps->setText("fps:100");
    view_fps->move(0, 20);

    set_fps = new QSpinBox(this);
    set_fps->move(200, 20);
    set_fps->setValue(25);
    set_fps->setRange(1, 200);

    sdl_render_width = ui.label->width();
    sdl_render_height = ui.label->height();
    view = XVideoView::Creat(XVideoView::SDL);
    view->Init(sdl_render_width, sdl_render_height, XVideoView::YUV420P,(void*)ui.label->winId());
    
    frame = av_frame_alloc();
    //图像参数
    frame->width = sdl_render_width;
    frame->height = sdl_render_height;
    frame->format = AV_PIX_FMT_YUV420P;
    frame->linesize[0] = sdl_render_width;
    frame->linesize[1] = sdl_render_width / 2;
    frame->linesize[2] = sdl_render_width / 2;
    int re = av_frame_get_buffer(frame, 0);
    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf));
        cout << buf << endl;
    }

    yuv = new unsigned char[sdl_render_width * sdl_render_height * pix_size];
    Mirror_yuv = new unsigned char[sdl_render_width * sdl_render_height * pix_size];
    Mirror_y = new unsigned char[sdl_render_width * sdl_render_height * pix_size];
    Mirror_u = new unsigned char[sdl_render_width * sdl_render_height * pix_size];
    Mirror_v = new unsigned char[sdl_render_width * sdl_render_height * pix_size];

    //startTimer(10);
    th_ = std::thread(&SdlQtRBG::Main, this);
}

void SdlQtRBG::timerEvent(QTimerEvent* ev)
{
    //yuv_file.read((char*)yuv, sdl_render_width * sdl_render_height * 1.5);
    
    yuv_file.read((char*)frame->data[0], sdl_render_width * sdl_render_height);
    yuv_file.read((char*)frame->data[1], sdl_render_width * sdl_render_height / 4);
    yuv_file.read((char*)frame->data[2], sdl_render_width * sdl_render_height / 4);

    //镜像播放
    uint8_t* y_plane = yuv;
    uint8_t* u_plane = yuv + sdl_render_width * sdl_render_height;
    uint8_t* v_plane = yuv + sdl_render_width * sdl_render_height + (sdl_render_width * sdl_render_height) / 4;

    for (int y = 0; y < sdl_render_height; y++)
    {
        for (int x = 0; x < sdl_render_width; x++)
        {
            Mirror_y[y * sdl_render_width + x] =
                y_plane[y * sdl_render_width + (sdl_render_width - 1 - x)];
        }
    }

    for (int y = 0; y < sdl_render_height / 2; y++)
    {
        for (int x = 0; x < sdl_render_width / 2; x++)
        {
            Mirror_u[y * sdl_render_width / 2 + x] =
                u_plane[y * sdl_render_width / 2 + (sdl_render_width / 2 - 1 - x)];
        }
    }

    for (int y = 0; y < sdl_render_height / 2; y++)
    {
        for (int x = 0; x < sdl_render_width / 2; x++)
        {
            Mirror_v[y * sdl_render_width / 2 + x] =
                v_plane[y * sdl_render_width / 2 + (sdl_render_width / 2 - 1 - x)];
        }
    }
    

    std::memcpy(Mirror_yuv, Mirror_y, sdl_render_width * sdl_render_height);
    std::memcpy(Mirror_yuv + sdl_render_width * sdl_render_height, Mirror_u, sdl_render_width * sdl_render_height / 4);
    std::memcpy(Mirror_yuv + sdl_render_width * sdl_render_height + sdl_render_width * sdl_render_height / 4, Mirror_v, sdl_render_width * sdl_render_height / 4);
    
    if (view->IsExit())
    {
        view->Close();
        exit(0);//需要加上这行代码才能关闭
    }

    view->DrawFrame(frame);
    //view->Draw(yuv);

}

void SdlQtRBG::resizeEvent(QResizeEvent* ev)
{
    ui.label->resize(size());
    ui.label->move(0, 0);   //加这行对齐左上角
    //view->SetScale(ui.label->width(), ui.label->height());
}

void SdlQtRBG::Main()
{
    while (!isExit_)
    {
        ViewS();
        //this_thread::sleep_for(10ms);
        //MSleep(10);
        if (fps > 0)
        {
            MSleep(1000 / fps);
        }
        else
        {
            MSleep(1000 / 25);
        }
        
    }
}

void SdlQtRBG::View()
{
    
    //循环播放
    if (yuv_file.tellg() == file_size)
    {
        yuv_file.seekg(0, ios::beg);
    }
    yuv_file.read((char*)frame->data[0], sdl_render_width * sdl_render_height);
    yuv_file.read((char*)frame->data[1], sdl_render_width * sdl_render_height / 4);
    yuv_file.read((char*)frame->data[2], sdl_render_width * sdl_render_height / 4);

    
    if (view->IsExit())
    {
        view->Close();
        exit(0);//需要加上这行代码才能关闭
    }

    view->DrawFrame(frame);

    stringstream ss;
    ss << "fps:" << view->render_fps();
    //只能在槽函数里调用
    view_fps->setText(ss.str().c_str());
    fps = set_fps->value();
}

SdlQtRBG::~SdlQtRBG()
{
    //等待渲染线程退出
    isExit_ = true;
    th_.join();
}

