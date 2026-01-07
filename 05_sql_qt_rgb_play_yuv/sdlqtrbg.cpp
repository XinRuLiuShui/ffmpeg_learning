#include "sdlqtrbg.h"
#include "SDL.h"
#include <QLabel>          // 加这一行
#include <fstream>
#include <iostream>
#include <QMessageBox>

using namespace std;

#pragma comment(lib,"SDL2.lib")
#undef main

static SDL_Window* sdl_screen = NULL;
static SDL_Renderer* sdl_render = NULL;
static SDL_Texture* texture = NULL;
static int sdl_render_width = 0;
static int sdl_render_height = 0;
static int pix_size = 2;
static unsigned char* yuv = NULL;
static ifstream yuv_file;

SdlQtRBG::SdlQtRBG(QWidget *parent)
    : QMainWindow(parent)
{
    yuv_file.open("400_300_25rgba2yuv.yuv",ios::binary);
    if (!yuv_file)
    {
        QMessageBox::information(this, "", "open yuv_file failed!");
        return;
    }
    ui.setupUi(this);
    sdl_render_width = ui.label->width();
    sdl_render_height = ui.label->height();
    /*sdl_render_width = 400;
    sdl_render_height = 300;
    ui.label->resize(sdl_render_width, sdl_render_height);*/
    SDL_Init(SDL_INIT_VIDEO);
    sdl_screen = SDL_CreateWindowFrom((void*)ui.label->winId());
    sdl_render = SDL_CreateRenderer(sdl_screen, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(sdl_render, SDL_PIXELFORMAT_IYUV,
        SDL_TEXTUREACCESS_STREAMING,
        sdl_render_width, sdl_render_height
    );
    if (!texture) {
        QMessageBox::information(this, "", SDL_GetError());
        return;
    }
    yuv = new unsigned char[sdl_render_width * sdl_render_height * pix_size];
    startTimer(10);
}

void SdlQtRBG::timerEvent(QTimerEvent* ev)
{
    yuv_file.read((char*)yuv, sdl_render_width * sdl_render_height * 1.5);
    SDL_UpdateTexture(texture, NULL, yuv, sdl_render_width);
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

