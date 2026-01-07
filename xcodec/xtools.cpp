#include "xtools.h"
#include <iostream>
#include <sstream>
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

using namespace std;


bool PrintErr(int re)
{
	if (re != 0)
	{
		char buf[1024];
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << buf << endl;
		return false;
	}
	return true;
}

void XThread::Start()
{
	unique_lock<mutex> lock(mtx_);
	static int i = 0;
	i++;
	index_ = i;
	is_exit_ = false;
	th_ = thread(&XThread::Main, this);
	stringstream ss;
	ss << "XThread::Start() " << index_ << " start";
	LOGINFO(ss.str());
}

void XThread::Stop()
{
	stringstream ss;
	ss << "XThread::Stop() " << index_ << " start";
	LOGINFO(ss.str());
	is_exit_ = true;
	if (th_.joinable())
	{
		th_.join();
	}
	ss.str("");
	ss << "XThread::Stop() " << index_ << " end";
	LOGINFO(ss.str());
}


void MSleep(unsigned int ms)
{
	auto beg = clock();
	for (int i = 0; i < ms; i++)
	{
		this_thread::sleep_for(1ms);
		if ((clock() - beg) / (CLOCKS_PER_SEC / 1000) >= ms)
		{
			break;
		}
	}
}

long long NowMs()
{
	return clock() / (CLOCKS_PER_SEC / 1000);
}

void XFreeFrame(AVFrame** frame)
{
	if (!frame || !(*frame))
	{
		return;
	}
	av_frame_free(frame);
}



XPara::XPara()
{
	para = avcodec_parameters_alloc();
	time_base = new AVRational();
}

XPara* XPara::Create()
{
	return new XPara();
}

XPara::~XPara()
{
	if (para)
	{
		avcodec_parameters_free(&para);
	}
	if (time_base)
	{
		delete time_base;
		time_base = nullptr;
	}
}

AVPacket* XAVPacketList::Pop()
{
	unique_lock<mutex> lock(mtx_);
	if (pkts_.empty())
	{
		return nullptr;
	}
	auto pkt = pkts_.front();
	pkts_.pop_front();
	return pkt;
	
}

void XAVPacketList::Push(AVPacket* pkt)
{
	unique_lock<mutex> lock(mtx_);
	//生成新的AVPacket 对象 引用计数+1;
	auto p = av_packet_alloc();
	av_packet_ref(p, pkt);//引用计数 减少数据复制，线程安全
	pkts_.push_back(p);

	//超出最大空间，清理数据，到关键帧位置
	if (pkts_.size() > max_packets_)
	{
		//处理第一帧
		if (pkts_.front()->flags & AV_PKT_FLAG_KEY)//关键帧
		{
			av_packet_free(&pkts_.front());//清理
			pkts_.pop_front();  //出队
			return;
		}
		//清理所有非关键帧之前的数据
		while (!pkts_.empty())
		{
			if (pkts_.front()->flags & AV_PKT_FLAG_KEY)//关键帧
			{
				return;
			}
			av_packet_free(&pkts_.front());//清理
			pkts_.pop_front();  //出队
		}
	}
}
