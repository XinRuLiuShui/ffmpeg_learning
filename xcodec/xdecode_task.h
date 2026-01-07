#pragma once
#include "xtools.h"
#include "xdecode.h"
class XCODEC_API XDecodeTask :
    public XThread
{
public:
    //打开解码器
    bool Open(AVCodecParameters* para);
    //责任链处理函数
    void Do(AVPacket* pkt) override;
    //主线程函数
    void Main() override;

    //返回当前需要渲染的frame，没有的话返回空
    //线程安全
    //返回结果需要用XFreeFrame释放
    AVFrame* GetFrame();

    void set_video_stream_index(int idx) { video_stream_index_ = idx; }
private:
    std::mutex mtx_;
    XDecode decode_;
    XAVPacketList pkt_list_;
    AVFrame* frame_ = nullptr;
    bool need_view_ = false;    //是否需要渲染，每帧只渲染一次
    int video_stream_index_ = -1;

};

