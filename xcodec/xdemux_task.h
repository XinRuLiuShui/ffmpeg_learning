#pragma once
#include "xtools.h"
#include "xdemux.h"

enum XSYN_TYPE
{
    XSYN_NONE = 0,  //不做同步
    XSYN_VIDEO = 1, //根据视频同步，不处理音频
};
class XCODEC_API XDemuxTask :
    public XThread
{
public:
    void Main();
    bool Open(std::string url, int timeout_ms = 3000);

    //复制视频参数，返回智能指针
    std::shared_ptr<XPara> CopyVideoPara();

    //复制音频参数，返回智能指针
    std::shared_ptr<XPara> CopyAudioPara();

    int video_index() { return demux_.video_index(); }
    int audio_index() { return demux_.audio_index(); }
    void set_syn_type(XSYN_TYPE type) { syn_type_ = type; }
private:
    XDemux demux_;
    std::string url_;
    int timeout_ms_ = 0;
    XSYN_TYPE syn_type_ = XSYN_NONE;
};

