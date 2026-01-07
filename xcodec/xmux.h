#pragma once
#include "xformat.h"
class XCODEC_API XMux :
    public XFormat
{
public:
    static AVFormatContext* Open(const char* url,
        AVCodecParameters* video_para = nullptr,
        AVCodecParameters* audio_para = nullptr);
    bool WriteHeader();
    bool Write(AVPacket* pkt);
    bool WriteTrailer();

    //音视频时间基数 
    void set_src_video_time_base(AVRational* tb);
    void set_src_audio_time_base(AVRational* tb);

    long long begin_video_pts = -1; //原视频开始时间
    long long begin_audio_pts = -1; //原音频开始时间

    int video_index_from_demux = -1;

    void set_video_index(int index) { video_index_ = index; }
    void set_audio_index(int index) { audio_index_ = index; }
    ~XMux();
private:
    AVRational* src_video_time_base_ = nullptr;
    AVRational* src_audio_time_base_ = nullptr;
};

