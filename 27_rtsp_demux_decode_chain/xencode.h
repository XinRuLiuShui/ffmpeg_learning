#pragma once
#include "xcodec.h"
#include <mutex>
#include <iostream>
#include <vector>
struct AVCodecContext;
struct AVFrame;
struct AVPacket;

class XEncode:public XCodec
{
public:


	//编码数据，线程安全	每次会创建avpacket
	//@para frame 空间由用户维护
	//@return 失败返回空指针，返回的avpacket，用户需要通过av_packer_free清理
	AVPacket* Encode(const AVFrame* frame);

	

	//返回所有编码缓存中的avpacket
	std::vector<AVPacket*> End();
};

