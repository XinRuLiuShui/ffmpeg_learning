#pragma once
#include "xtools.h"
#include "xdemux.h"
class XDemuxTask :
    public XThread
{
public:
    void Main();
    bool Open(std::string url, int timeout_ms = 3000);

    //复制视频参数，返回智能指针
    std::shared_ptr<XPara> CopyVideoPara();

    //复制音频参数，返回智能指针
    std::shared_ptr<XPara> CopyAudioPara();
private:
    XDemux demux_;
    std::string url_;
    int timeout_ms_ = 0;
};

