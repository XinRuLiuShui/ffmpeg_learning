#include "sdlqtrbg.h"
#include "SDL.h"
#include <QLabel>          // 加这一行

#pragma comment(lib,"SDL2.lib")
#undef main

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
    texture = SDL_CreateTexture(sdl_render, SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STREAMING,
        sdl_render_width, sdl_render_height
    );
    rgb = new unsigned char[sdl_render_width * sdl_render_height * pix_size];
    startTimer(20);
}

void SdlQtRBG::timerEvent(QTimerEvent* ev)
{
    temp += dir;
    if (temp == 0 || temp == 255)
    {
        dir = -dir;
    }
    for (int j = 0; j < sdl_render_height; j++)
    {
        int b = j * sdl_render_width * pix_size;
        for (int i = 0; i < sdl_render_width * pix_size; i += pix_size)
        {
            rgb[b + i] = temp;		//R
            rgb[b + i + 1] = 0;	//G
            rgb[b + i + 2] = 0;	//B
            rgb[b + i + 3] = 0;	//A
        }
    }
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

