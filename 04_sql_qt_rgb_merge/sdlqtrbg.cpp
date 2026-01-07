#include "sdlqtrbg.h"
#include "SDL.h"
#include <QLabel>          // 加这一行
#include <QMessageBox>

#pragma comment(lib,"SDL2.lib")

static SDL_Window* sdl_screen = NULL;
static SDL_Renderer* sdl_render = NULL;
static SDL_Texture* texture = NULL;
static int sdl_render_width = 0;
static int sdl_render_height = 0;
static int pix_size = 4;
static int temp = 255;
static int dir = -1;
static unsigned char* rgb = NULL;

SdlQtRBG::SdlQtRBG(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    sdl_render_width = ui.label->width();
    sdl_render_height = ui.label->height();
    
    SDL_Init(SDL_INIT_VIDEO);
    sdl_screen = SDL_CreateWindowFrom((void*)ui.label->winId());
    sdl_render = SDL_CreateRenderer(sdl_screen, -1, SDL_RENDERER_ACCELERATED);
    
    QImage img1("E:/FFMPEG/code/bin/x86/03.jpg");
    QImage img2("E:/FFMPEG/code/bin/x86/02.jpg");
    QImage img3("E:/FFMPEG/code/bin/x86/01.jpg");
    if (img1.isNull() || img2.isNull() || img3.isNull())
    {
        QMessageBox::information(this, "", "open jpg failed!");
        return;
    }

    int out_w = img1.width() + img2.width();
    int out_h = img1.height();
    if (out_h<img2.height())
    {
        out_h = img2.height();
    }
    sdl_render_width = out_w;
    sdl_render_height = out_h;
    resize(sdl_render_width, sdl_render_height);
    ui.label->move(0, 0);
    ui.label->resize(sdl_render_width, sdl_render_height);

    

    texture = SDL_CreateTexture(sdl_render, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        sdl_render_width, sdl_render_height
    );
    rgb = new unsigned char[sdl_render_width * sdl_render_height * pix_size];
    
    memset(rgb, 0, sdl_render_width * sdl_render_height * pix_size);

    for (int i = 0; i < sdl_render_width; i++)
    {
        int b = i * sdl_render_width * pix_size;
        if (i < img1.height())
        {
            memcpy(rgb + b, img1.scanLine(i), img1.width() * pix_size);
        }
        b += img1.width() * pix_size;
        if (i < img2.height())
        {
            memcpy(rgb + b, img2.scanLine(i), img2.width() * pix_size);
        }
        else if (i<img1.height())
        {
            //memcpy(rgb + b, img2.scanLine(i-img2.height()), img2.width() * pix_size);
            memcpy(rgb + b, img3.scanLine(i - img3.height()), img3.width() * pix_size);
        }

    }   

    QImage out(rgb, sdl_render_width, sdl_render_height, QImage::Format_ARGB32);
    out.save("out.jpg");

    startTimer(10);
}

void SdlQtRBG::timerEvent(QTimerEvent* ev)
{
    //temp += dir;
    //if (temp == 0 || temp == 255)
    //{
    //    dir = -dir;
    //}
    //for (int j = 0; j < sdl_render_height; j++)
    //{
    //    int b = j * sdl_render_width * pix_size;
    //    for (int i = 0; i < sdl_render_width * pix_size; i += pix_size)
    //    {
    //        //rgb[b + i] = 0;		//R
    //        //rgb[b + i + 1] = 0;	//G
    //        //rgb[b + i + 2] = temp;	//B
    //        //rgb[b + i + 3] = temp;	//A
    //    }
    //}

    SDL_UpdateTexture(texture, NULL, rgb, sdl_render_width * 4);
    SDL_RenderClear(sdl_render);

    SDL_Rect sdl_rect;
    sdl_rect.x = 0;
    sdl_rect.y = 0;
    sdl_rect.w = sdl_render_width;
    sdl_rect.h = sdl_render_height;

    SDL_RenderCopy(sdl_render, texture, NULL, &sdl_rect);

    SDL_RenderPresent(sdl_render);
}

SdlQtRBG::~SdlQtRBG()
{}

