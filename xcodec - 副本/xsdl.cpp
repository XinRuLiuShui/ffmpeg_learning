#include "xsdl.h"
#include "SDL.h"
#include <iostream>
using namespace std;
#pragma comment(lib,"SDL2.lib")

static bool InitVideo()
{
    static bool is_first = true;
    static mutex mux;
    unique_lock<mutex> sdl_lock(mux);
    if (!is_first)
    {
        return true;
    }
    is_first = false;
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        cout << SDL_GetError() << endl;
        return false;
    }
    //设置缩放算法，抗锯齿
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    return true;
}

bool XSDL::Init(int w, int h, Format fmt)
{
    if (w <= 0 || h <= 0)
    {
        return false;
    }
    //初始化SDL库
    InitVideo();
    //确保线程安全
    unique_lock<mutex> sdl_locl(mtx_);
    width_ = w;
    height_ = h;
    fmt_ = fmt;

    //材质要在渲染器之前清理
    if (texture_)
    {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
    if (render_)
    {
        SDL_DestroyRenderer(render_);
        render_ = nullptr;
    }

    //创建窗口
    if (!win_)
    {
        if (!win_id_)
        {
            win_ = SDL_CreateWindow(
                "",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                w,
                h,
                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
            );
        }
        else
        {
            win_ = SDL_CreateWindowFrom(win_id_);
        }
    }
    if(!win_)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }

   
    //创建渲染器
    render_ = SDL_CreateRenderer(win_, -1, SDL_RENDERER_ACCELERATED);
    if (!render_)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }

    
    //创建材质 显存
    /*unsigned int sdl_fmt = SDL_PIXELFORMAT_RGBA8888;*/
    unsigned int sdl_fmt = SDL_PIXELFORMAT_IYUV;
    switch (fmt)
    {
    case XVideoView::RGBA:
        sdl_fmt = SDL_PIXELFORMAT_RGBA32;
        break;
    case XVideoView::BGRA:
        sdl_fmt = SDL_PIXELFORMAT_BGRA32;
        break;
    case XVideoView::ARGB:
        sdl_fmt = SDL_PIXELFORMAT_ARGB32;
        break;
    case XVideoView::YUV420P:
        sdl_fmt = SDL_PIXELFORMAT_IYUV;
        break;
    case XVideoView::RGB24:
        sdl_fmt = SDL_PIXELFORMAT_RGB24;
        break;
    case XVideoView::NV12:
        sdl_fmt = SDL_PIXELFORMAT_NV12;
        break;
    default:
        break;
    }
    texture_ = SDL_CreateTexture(render_,
        sdl_fmt,
        SDL_TEXTUREACCESS_STREAMING,
        w,
        h
    );
    if (!texture_)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }
    return true;
}

void XSDL::Close()
{
    //确保线程安全
    unique_lock<mutex> sdl_locl(mtx_);

    if (texture_)
    {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
    if (render_)
    {
        SDL_DestroyRenderer(render_);
        render_ = nullptr;
    }
    if (win_)
    {
        SDL_DestroyWindow(win_);
        win_ = nullptr;
    }
}

bool XSDL::IsExit()
{
    SDL_Event ev;
    SDL_WaitEventTimeout(&ev, 1);
    if (ev.type == SDL_QUIT)
    {
        return true;
    }
    return false;
}

bool XSDL::Draw(const unsigned char* data, int linesize)
{
    if (!data)
    {
        return false;
    }

    //确保线程安全
    unique_lock<mutex> sdl_locl(mtx_);
    if (!win_ || !render_ || !texture_ || width_ <= 0 || height_ <= 0)
    {
        return false;
    }

    if (linesize <= 0)
    {
        switch (fmt_)
        {
        case XVideoView::RGBA:
        case XVideoView::ARGB:
            linesize = width_ * 4;
            break;
        case XVideoView::YUV420P:
            linesize = width_;
            break;
        case XVideoView::RGB24:
            linesize = width_ * 3;
            break;
        default:
            break;
        }
    }

    if (linesize <= 0)
    {
        return false;
    }

    auto re = SDL_UpdateTexture(texture_, NULL, data,
        linesize  //一行的字节数
    );

    if (re != 0)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }

    SDL_RenderClear(render_);

    //if (scale_w_ <= 0)
    //{
    //    scale_w_ = width_;
    //}
    //if (scale_h_ <= 0)
    //{
    //    scale_h_ = height_;
    //}

    SDL_Rect rect;
    SDL_Rect* prect = nullptr;
    if (scale_w_ > 0)
    {
        rect.x = 0;
        rect.y = 0;
        rect.w = scale_w_;
        rect.h = scale_h_;
        prect = &rect;
    }
    

    re = SDL_RenderCopy(render_, texture_, NULL, prect);
    if (re != 0)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }

    SDL_RenderPresent(render_);
    return true;
}

bool XSDL::Draw(const unsigned char* y, int y_pitch, const unsigned char* u, int u_pitch, const unsigned char* v, int v_pitch)
{
    if (!y || !u || !v)
    {
        return false;
    }

    //确保线程安全
    unique_lock<mutex> sdl_locl(mtx_);
    if (!win_ || !render_ || !texture_ || width_ <= 0 || height_ <= 0)
    {
        return false;
    }


    auto re = SDL_UpdateYUVTexture(texture_, NULL,
        y,y_pitch,
        u, u_pitch,
        v, v_pitch
    );

    if (re != 0)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }

    SDL_RenderClear(render_);

    //if (scale_w_ <= 0)
    //{
    //    scale_w_ = width_;
    //}
    //if (scale_h_ <= 0)
    //{
    //    scale_h_ = height_;
    //}

    SDL_Rect rect;
    SDL_Rect* prect = nullptr;
    if (scale_w_ > 0)
    {
        rect.x = 0;
        rect.y = 0;
        rect.w = scale_w_;
        rect.h = scale_h_;
        prect = &rect;
    }


    re = SDL_RenderCopy(render_, texture_, NULL, prect);
    if (re != 0)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }

    SDL_RenderPresent(render_);
    return true;
}
bool XSDL::Draw(const unsigned char* y, int y_pitch,
    const unsigned char* uv, int uv_pitch)
{
    if (!y || !uv)
        return false;

    unique_lock<mutex> sdl_locl(mtx_);
    if (!win_ || !render_ || !texture_ || width_ <= 0 || height_ <= 0)
        return false;

    uint8_t* pixels[2] = { 0 };
    int pitches[2] = { 0 };

    // ★ NV12: plane0 = Y, plane1 = UV
    if (SDL_LockTexture(texture_, nullptr,
        (void**)&pixels[0],
        &pitches[0]) != 0)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }

    // Y 平面
    for (int i = 0; i < height_; i++)
    {
        memcpy(pixels[0] + i * pitches[0],
            y + i * y_pitch,
            width_);
    }

    // UV 平面在 plane1
    pixels[1] = pixels[0] + pitches[0] * height_;
    pitches[1] = pitches[0];

    for (int i = 0; i < height_ / 2; i++)
    {
        memcpy(pixels[1] + i * pitches[1],
            uv + i * uv_pitch,
            width_);
    }

    SDL_UnlockTexture(texture_);

    SDL_RenderClear(render_);

    SDL_Rect rect;
    SDL_Rect* prect = nullptr;
    if (scale_w_ > 0)
    {
        rect.x = 0;
        rect.y = 0;
        rect.w = scale_w_;
        rect.h = scale_h_;
        prect = &rect;
    }

    if (SDL_RenderCopy(render_, texture_, NULL, prect) != 0)
    {
        cerr << SDL_GetError() << endl;
        return false;
    }

    SDL_RenderPresent(render_);
    return true;
}
