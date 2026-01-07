#pragma once
#include "xformat.h"
class XCODEC_API XDemux :
    public XFormat
{
public:
    //打开解封装,解封装地址支持rtsp,失败返回空指针
    AVFormatContext* Open(const char* url);
    bool Read(AVPacket* pkt);
    bool Seek(long long pts, int stream_index);
};

