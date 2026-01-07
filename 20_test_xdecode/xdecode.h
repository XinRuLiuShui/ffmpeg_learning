#pragma once
#include "xcodec.h"
#include <mutex>
#include <iostream>
#include <vector>
struct AVCodecContext;
struct AVFrame;
struct AVPacket;

class XDecode :
    public XCodec
{
public:
    bool Send(AVPacket* pkt);
    bool Recv(AVFrame* frame);
    bool InitHWDevice(AVHWDeviceType type);

    //返回所有编码缓存中的AVFrame
    std::vector<AVFrame*> End();

private:
    AVBufferRef* hw_device_ctx_ = nullptr;  // 硬件设备上下文
    AVPixelFormat hw_pix_fmt_ = AV_PIX_FMT_NV12; // 硬件输出像素格式

};

