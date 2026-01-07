#pragma once
#include "xtools.h"
#include "xmux.h"
class XCODEC_API XMuxTask :
    public XThread
{
public:
    void Main() override;
    /// <summary>
    /// 打开封装
    /// </summary>
    /// <param name="url">输出的地址</param>
    /// <param name="video_para">视频参数</param>
    /// <param name="video_time_base">视频的时间基数</param>
    /// <param name="audio_para">音频参数</param>
    /// <param name="audio_time_base">音频的时间基数</param>
    /// <returns></returns>
    bool Open(const char* url,
        AVCodecParameters* video_para = nullptr,
        AVRational* video_time_base = nullptr,
        AVCodecParameters* audio_para = nullptr,
        AVRational* audio_time_base = nullptr
    );

    void Do(AVPacket* pkt);
    void set_video_index(int index) { mux_.set_video_index(index); }
    void set_audio_index(int index) { mux_.set_audio_index(index); }
    void set_video_index_from_demux(int index) { mux_.video_index_from_demux = index; }
private:
    XMux mux_;
    XAVPacketList pkt_list_;
    std::mutex mtx_;
};

