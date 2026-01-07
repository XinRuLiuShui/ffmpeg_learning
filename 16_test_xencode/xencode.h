#pragma once
#include <mutex>
#include <iostream>
#include <vector>
struct AVCodecContext;
struct AVFrame;
struct AVPacket;

class XEncode
{
public:

	//创建编码上下文
	//@para codec_id 编码器id号对应ffmpeg
	//@return 成功返回编码器上下文，失败返回空指针
	static AVCodecContext* Creat(int codec_id);	//设为静态，说明该函数属于类,不需要实例化对象就可以使用


	//设置对象的编码器上下文，上下文传递到对象中，资源由XEncode维护
	//要加锁，保证线程的安全
	//@para c 编码器上下文，如果他不为空指针的话,则先清理资源
	void set_c(AVCodecContext* c);

	//设置编码参数，线程安全
	bool SetOpt(const char* key, const char* val);
	bool SetOpt(const char* key, int val);

	//打开编码器，线程安全
	bool Open();

	//编码数据，线程安全	每次会创建avpacket
	//@para frame 空间由用户维护
	//@return 失败返回空指针，返回的avpacket，用户需要通过av_packer_free清理
	AVPacket* Encode(const AVFrame* frame);

	//创建avframe，同样由用户维护,需要由调用者释放av_frame_free
	AVFrame* CreatFrame();

	//返回所有编码缓存中的avpacket
	std::vector<AVPacket*> End();
private:
	AVCodecContext* c_ = nullptr;	//编码上下文
	std::mutex mux_;	//保护编码器上下文
};

