#pragma once
#include "xformat.h"
class XDemux :
    public XFormat
{
public:
    //打开解封装，解封装地址支持rtsp失败，返回空指针
    static AVFormatContext* Open(const char* url);
    bool Read(AVPacket* pkt);
};

