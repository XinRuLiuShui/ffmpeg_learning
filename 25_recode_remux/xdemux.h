#pragma once
#include "xformat.h"
class XDemux :
    public XFormat
{
public:
    //打开解封装,解封装地址支持rtsp,失败返回空指针
    static AVFormatContext* Open(const char* url);
    bool Read(AVPacket* pkt);
    bool Seek(long long pts, int stream_index);
};

