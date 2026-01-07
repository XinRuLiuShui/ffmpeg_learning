#pragma once
#include "xtools.h"
#include "xdecode.h"
class XDecodeTask :
    public XThread
{
public:
    //打开解码器
    bool Open(AVCodecParameters* para);
    //责任链处理函数
    void Do(AVPacket* pkt) override;
    //主线程函数
    void Main() override;

    
private:
    std::mutex mtx_;
    XDecode decode_;
    XAVPacketList pkt_list_;
    AVFrame* frame_ = nullptr;
};

