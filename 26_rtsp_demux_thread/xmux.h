#pragma once
#include "xformat.h"
class XMux :
    public XFormat
{
public:
    static AVFormatContext* Open(const char* url);
    bool WriteHeader();
    bool Write(AVPacket* pkt);
    bool WriteTrailer();
};

