//////////////////////////////////  @版权说明  //////////////////////////////////////////////////
///						Jiedi(China nanjing)Ltd.                                    
/// @版权说明 代码和课程版权有夏曹俊所拥有并已经申请著作权，此代码可用作为学习参考并可在项目中使用，
/// 课程中涉及到的其他开源软件，请遵守其相应的授权
/// 课程源码不可以直接转载到公开的博客，或者其他共享平台，不可以用以制作在线课程。
/// 课程中涉及到的其他开源软件，请遵守其相应的授权               
/////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////  源码说明  //////////////////////////////////////////////////
/// 项目名称      : FFmpeg 4.2 从基础实战-多路H265监控录放开发 实训课
/// Contact       : xiacaojun@qq.com
///  博客   :				http://blog.csdn.net/jiedichina
///	视频课程 : 网易云课堂	http://study.163.com/u/xiacaojun		
///			   腾讯课堂		https://jiedi.ke.qq.com/				
///			   csdn学院		http://edu.csdn.net/lecturer/lecturer_detail?lecturer_id=961	
///           51cto学院		https://edu.51cto.com/sd/d3b6d
///			   老夏课堂		http://www.laoxiaketang.com 
/// 更多资料请在此网页下载  http://ffmpeg.club
/// ！！！请加入课程qq群 【639014264】与同学交流和下载资料 
/// 微信公众号: jiedi2007
/// 头条号	 : xiacaojun
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// 课程交流qq群 639014264  //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "xsdl.h"
#include <thread>
#include <iostream>
using namespace std;
extern "C"
{
#include <libavcodec/avcodec.h>
}
#pragma comment(lib,"avutil.lib")

void MSleep(unsigned int ms)
{
	auto beg = clock();
	for (int i = 0; i < ms; i++)
	{
		this_thread::sleep_for(1ms);
		if ((clock() - beg) / (CLOCKS_PER_SEC / 1000) >= ms)
			break;
	}
}
long long NowMs()
{
	return clock() / (CLOCKS_PER_SEC / 1000);
}
AVFrame* XVideoView::Read()
{
	if (width_ <= 0 || height_ <= 0 || !ifs_)return NULL;
	//AVFrame空间已经申请，如果参数发生变化，需要释放空间
	if (frame_)
	{
		if (frame_->width != width_
			|| frame_->height != height_ 
			|| frame_->format != fmt_)
		{
			//释放AVFrame对象空间，和buf引用计数减一
			av_frame_free(&frame_);
		}
	}
	if (!frame_)
	{
		//分配对象空间和像素空间
		frame_ = av_frame_alloc();
		frame_->width = width_;
		frame_->height = height_;
		frame_->format = fmt_;
		frame_->linesize[0] = width_ * 4;
		if (frame_->format == AV_PIX_FMT_YUV420P)
		{
			frame_->linesize[0] = width_; // Y
			frame_->linesize[1] = width_ / 2;//U
			frame_->linesize[2] = width_ / 2;//V
		}

		//生成AVFrame空间，使用默认对齐
		auto re = av_frame_get_buffer(frame_, 0);
		if (re != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(re, buf, sizeof(buf) - 1);
			cout << buf << endl;
			av_frame_free(&frame_);
			return NULL;

		}
	}
	if (!frame_)return NULL;

	//读取一帧数据
	if (frame_->format == AV_PIX_FMT_YUV420P)
	{
		ifs_.read((char*)frame_->data[0], 
			frame_->linesize[0] * height_);	//Y
		ifs_.read((char*)frame_->data[1],
			frame_->linesize[1] * height_/2);	//U
		ifs_.read((char*)frame_->data[2], 
			frame_->linesize[2] * height_/2);	//V
	}
	else	//RGBA ARGB BGRA 32
	{
		ifs_.read((char*)frame_->data[0], frame_->linesize[0] * height_);
	}

	if (ifs_.gcount() == 0)
		return NULL;
	return frame_;




}

//打开文件
bool XVideoView::Open(std::string filepath)
{
	if (ifs_.is_open())
	{
		ifs_.close();
	}
	ifs_.open(filepath,ios::binary);
	return ifs_.is_open();
}
XVideoView* XVideoView::Create(RenderType type)
{
	switch (type)
	{
	case XVideoView::SDL:
		return new XSDL();
		break;
	default:
		break;
	}
	return nullptr;
}
XVideoView::~XVideoView()
{
	if (cache_)
		delete cache_;
	cache_ = nullptr;
}
bool XVideoView::DrawFrame(AVFrame* frame)
{
	if (!frame || !frame->data[0])return false;
	count_++;
	if (beg_ms_ <= 0)
	{
		beg_ms_ = clock();
	}
	//计算显示帧率
	else if((clock() - beg_ms_)/(CLOCKS_PER_SEC/1000)>=1000) //一秒计算一次fps
	{
		render_fps_ = count_;
		count_ = 0;
		beg_ms_ = clock();
	}
	int linesize = 0;
	switch (frame->format)
	{
	case AV_PIX_FMT_YUV420P:
		return Draw(frame->data[0], frame->linesize[0],//Y
			frame->data[1], frame->linesize[1],	//U
			frame->data[2], frame->linesize[2]	//V
		);
	case AV_PIX_FMT_NV12:
		if (!cache_)
		{
			cache_ = new unsigned char[4096 * 2160 * 1.5];
		}
		linesize = frame->width;
		if (frame->linesize[0] == frame->width)
		{
			memcpy(cache_, frame->data[0], frame->linesize[0] * frame->height); //Y
			memcpy(cache_ + frame->linesize[0] * frame->height, frame->data[1], frame->linesize[1] * frame->height / 2); //UV
		}
		else //逐行复制
		{
			for (int i = 0; i < frame->height; i++) //Y
			{
				memcpy(cache_ + i * frame->width,
					frame->data[0] + i * frame->linesize[0],
					frame->width
				);
			}
			for (int i = 0; i < frame->height/2; i++)  //UV
			{
				auto p = cache_ + frame->height * frame->width;// 移位Y
				memcpy(p + i * frame->width,
					frame->data[1] + i * frame->linesize[1],
					frame->width
				);
			}
		}

		//frame->data[0] + frame->data[1]
		return Draw(cache_, linesize);
	case AV_PIX_FMT_BGRA:
	case AV_PIX_FMT_ARGB:
	case AV_PIX_FMT_RGBA:
		return Draw(frame->data[0], frame->linesize[0]);
	default:
		break;
	}
	return false;
}