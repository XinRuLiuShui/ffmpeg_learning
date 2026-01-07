#pragma once
#include "xtools.h"
#include "xdemux.h"
class XDemuxTask :
    public XThread
{
public:
    void Main();
    bool Open(std::string url, int timeout_ms = 3000);

private:
    XDemux demux_;
    std::string url_;
    int timeout_ms_ = 0;
};

