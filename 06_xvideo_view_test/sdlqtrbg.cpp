#include "sdlqtrbg.h"
#include "xvideo_view.h"
//#include "SDL.h"
#include <QLabel>          // 加这一行
#include <fstream>
#include <iostream>
#include <QMessageBox>

using namespace std;

//#pragma comment(lib,"SDL2.lib")
//#undef main

//static SDL_Window* sdl_screen = NULL;
//static SDL_Renderer* sdl_render = NULL;
//static SDL_Texture* texture = NULL;
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
    ui.setupUi(this);
    
    sdl_render_width = ui.label->width();
    sdl_render_height = ui.label->height();
    view = XVideoView::Creat(XVideoView::SDL);
    view->Init(sdl_render_width, sdl_render_height, XVideoView::YUV420P);
    
    //模拟重复调用
    //view->Init(sdl_render_width, sdl_render_height, XVideoView::YUV420P, (void*)ui.label->winId());

    //view1 = XVideoView::Creat(XVideoView::SDL);
    //view1->Init(sdl_render_width, sdl_render_height, XVideoView::YUV420P);
    //
    //view2 = XVideoView::Creat(XVideoView::SDL);
    //view2->Init(sdl_render_width, sdl_render_height, XVideoView::YUV420P);

    ///*sdl_render_width = 400;
    //sdl_render_height = 300;
    //ui.label->resize(sdl_render_width, sdl_render_height);*/
    //SDL_Init(SDL_INIT_VIDEO);
    //sdl_screen = SDL_CreateWindowFrom((void*)ui.label->winId());
    //sdl_render = SDL_CreateRenderer(sdl_screen, -1, SDL_RENDERER_ACCELERATED);
    //texture = SDL_CreateTexture(sdl_render, SDL_PIXELFORMAT_IYUV,
    //    SDL_TEXTUREACCESS_STREAMING,
    //    sdl_render_width, sdl_render_height
    //);
    //if (!texture) {
    //    QMessageBox::information(this, "", SDL_GetError());
    //    return;
    //}
    yuv = new unsigned char[sdl_render_width * sdl_render_height * pix_size];
    Mirror_yuv = new unsigned char[sdl_render_width * sdl_render_height * pix_size];
    Mirror_y = new unsigned char[sdl_render_width * sdl_render_height * pix_size];
    Mirror_u = new unsigned char[sdl_render_width * sdl_render_height * pix_size];
    Mirror_v = new unsigned char[sdl_render_width * sdl_render_height * pix_size];

    startTimer(10);
}

void SdlQtRBG::timerEvent(QTimerEvent* ev)
{
    yuv_file.read((char*)yuv, sdl_render_width * sdl_render_height * 1.5);
    
    
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
    //std::memcpy(Mirror_yuv, v_plane, sdl_render_width * sdl_render_height / 4);
    //std::memcpy(Mirror_yuv + sdl_render_width * sdl_render_height / 4, u_plane, sdl_render_width * sdl_render_height / 4);


    std::memcpy(Mirror_yuv, Mirror_y, sdl_render_width * sdl_render_height);
    std::memcpy(Mirror_yuv + sdl_render_width * sdl_render_height, Mirror_u, sdl_render_width * sdl_render_height / 4);
    std::memcpy(Mirror_yuv + sdl_render_width * sdl_render_height + sdl_render_width * sdl_render_height / 4, Mirror_v, sdl_render_width * sdl_render_height / 4);
    
    if (view->IsExit())
    {
        view->Close();
        exit(0);//需要加上这行代码才能关闭
    }
    view->Draw(yuv);

    //view1->Draw(yuv);
    //view2->Draw(yuv);
    /*SDL_UpdateTexture(texture, NULL, yuv, sdl_render_width);
    SDL_RenderClear(sdl_render);

    SDL_Rect sdl_rect;
    sdl_rect.x = 0;
    sdl_rect.y = 0;
    sdl_rect.w = sdl_render_width;
    sdl_rect.h = sdl_render_height;

    SDL_RenderCopy(sdl_render, texture, NULL, &sdl_rect);

    SDL_RenderPresent(sdl_render);*/
}

void SdlQtRBG::resizeEvent(QResizeEvent* ev)
{
    ui.label->resize(size());
    //view->SetScale(ui.label->width(), ui.label->height());
}

SdlQtRBG::~SdlQtRBG()
{}

