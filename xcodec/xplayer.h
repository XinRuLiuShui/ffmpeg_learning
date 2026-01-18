#pragma once
#include "xtools.h"
#include "xdemux_task.h"
#include "xdecode_task.h"
#include "xvideo_view.h"
class XCODEC_API XPlayer :
    public XThread
{
public:
    bool Open(const char* url, void* winid);
    void Main() override;
    //责任链处理函数
    void Do(AVPacket* pkt) override;

    void Start(void);

    //渲染视频播放音频
    void Update(void);
protected:
    XDecodeTask video_decode_task_;
    XDecodeTask audio_decode_task_;
    XDemuxTask demux_task_;
    XVideoView* view_ = nullptr;
};

