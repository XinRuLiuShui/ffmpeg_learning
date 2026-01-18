#include "xaudio_play.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <SDL.h>
extern "C" {
#include <libavformat/avformat.h>
}
using namespace std;

class CXAudioPlay :public XAudioPlay
{
public:

    bool Open(XAudioSpec& spec) 
    {
        this->spec_ = spec;
        // 退出上一次音频
        SDL_QuitSubSystem(SDL_INIT_AUDIO);

        SDL_AudioSpec sdl_spec;
        sdl_spec.freq = spec.freq;
        sdl_spec.format = spec.format;
        sdl_spec.channels = spec.channels;
        sdl_spec.samples = spec.samples;
        sdl_spec.silence = 0;
        sdl_spec.userdata = this;
        sdl_spec.callback = AudioCallback;
        if (SDL_OpenAudio(&sdl_spec, nullptr) < 0) {
            std::cerr << SDL_GetError() << std::endl;
            return false;
        }
        //Play
        SDL_PauseAudio(0);
        return true;
    }

    void Callback(unsigned char* stream, int len)
    {
        SDL_memset(stream, 0, len);

        std::unique_lock<std::mutex> lock(mtx_);
        if (audio_datas_.empty()) return;

        auto buf = audio_datas_.front();
        // 1. buf大于stream缓冲：offset记录位置
        // 2. buf小于stream缓冲：拼接
        int mixed_size = 0;      // 已经处理的字节数
        int need_size = len;     // 需要处理的字节数

        cur_pts_ = buf.pts;
        last_ms_ = NowMs();
        while (mixed_size < len) 
        {
            if (audio_datas_.empty()) break;

            buf = audio_datas_.front();
            int size = static_cast<int>(buf.data.size()) - buf.offset; // 剩余未处理的数据
            if (size > need_size) size = need_size;

            SDL_MixAudio(stream + mixed_size,
                buf.data.data() + buf.offset,
                size,
                volume_);

            need_size -= size;
            mixed_size += size;
            buf.offset += size;

            if (buf.offset >= buf.data.size())
                audio_datas_.pop_front();
        }
    }

    long long cur_pts(void) override
    {
        double ms = 0;
        if (last_ms_ > 0)
        {
            ms = NowMs() - last_ms_;    //距离上次写入缓冲的时间
        }
        //毫秒换算成pts
        if (time_base_ > 0)
        {
            ms = ms / (double)1000 / (double)time_base_;
        }
        return cur_pts_ + ms;
    }

    void close()
    {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        std::unique_lock<std::mutex> lock(mtx_);
        audio_datas_.clear(); 
    }
private:
    long long cur_pts_ = 0;	//当前播放位置pts
    long long last_ms_ = 0;  //上次的时间戳
};
XAudioPlay* XAudioPlay::Instance()
{
	static CXAudioPlay cx;
	return &cx;
}

bool XAudioPlay::Open(AVCodecParameters* para)
{
    XAudioSpec spec;
    spec.channels = para->channels;
    spec.freq = para->sample_rate;
    //AVSampleFormat
    switch (para->format)
    {   
    case AV_SAMPLE_FMT_S16:     ///< signed 16 bits
    case AV_SAMPLE_FMT_S16P:    ///< signed 16 bits, planar
        spec.format = AUDIO_S16;
        break; 
    case AV_SAMPLE_FMT_S32:         ///< signed 32 bits
    case AV_SAMPLE_FMT_S32P:       ///< signed 32 bits, planar
        spec.format = AUDIO_S32;
        break;
    case AV_SAMPLE_FMT_FLT:         ///< float
    case AV_SAMPLE_FMT_FLTP:        ///< float, planar
        spec.format = AUDIO_F32;
        break;
    default:
        break;
    }
    return Open(spec);
}

bool XAudioPlay::Open(XPara& para)
{
    if (para.time_base->num > 0)
    {
        time_base_ = (double)para.time_base->den / (double)para.time_base->num;
    }
    return Open(para.para);
}

void XAudioPlay::Push(AVFrame* frame)
{
    vector<unsigned char> buf;
    int sample_size = 4;
    int channels = frame->channels;
    unsigned char* L = frame->data[0];
    unsigned char* R = frame->data[1];
    unsigned char* dat = nullptr;
    switch (frame->format)
    {
    //case AV_SAMPLE_FMT_S16P:    ///< signed 16 bits, planar
    //    break;
    
    case AV_SAMPLE_FMT_S32P:       ///< signed 32 bits, planar
    case AV_SAMPLE_FMT_FLTP:        ///< float, planar
        buf.resize(frame->linesize[0]);
        dat = buf.data();
        //  LLLL RRRR
        //  LR LR LR LR 
        for (int i = 0; i < frame->nb_samples; i++)
        {
            memcpy(dat + i * sample_size * channels, L + i * sample_size, sample_size);
            memcpy(dat + i * sample_size * channels + sample_size, R + i * sample_size, sample_size);
        }
        Push(dat, frame->linesize[0], frame->pts);
        break;
    default:
        Push(frame->data[0], frame->linesize[0], frame->pts);
        break;
    }
    
}

XAudioPlay::XAudioPlay()
{

}
