#include "xdecode.h"
#include <iostream>
#include <fstream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")//使用av_strerror需要添加这个库
using namespace std;

bool XDecode::Send(AVPacket* pkt)
{
	unique_lock<mutex>lock(mux_);
	if (!c_)
	{
		return false;
	}
	int ret = avcodec_send_packet(c_, pkt);
	if (ret != 0)
	{
		return false;
	}
	return true;
}

bool XDecode::Recv(AVFrame* frame)
{
	unique_lock<mutex>lock(mux_);
	if (!c_)
	{
		return false;
	}
	int ret = avcodec_receive_frame(c_, frame);

	if (ret == 0)
	{
        if (c_->hw_device_ctx)
        {
            //AVFrame* hw_frame = av_frame_alloc();
            //av_hwframe_transfer_data(hw_frame, frame, 0);
            //frame = hw_frame;
            AVFrame* sw = av_frame_alloc();
            if (!sw) return false;

            ret = av_hwframe_transfer_data(sw, frame, 0);
            if (ret < 0)
            {
                av_frame_free(&sw);
                return false;
            }

            // 用 sw 替换调用者传入 frame 的内容
            av_frame_unref(frame);
            av_frame_move_ref(frame, sw);

            av_frame_free(&sw);
        }
		return true;
	}
	else
	{
		return false;
	}
	return false;
}


bool XDecode::InitHWDevice(AVHWDeviceType type)
{
    int re = av_hwdevice_ctx_create(&hw_device_ctx_, type, NULL, NULL, 0);
    if (re < 0)
    {
        std::cerr << "Failed to create HW device: " << std::endl;
        return false;
    }
    c_->hw_device_ctx = av_buffer_ref(hw_device_ctx_);
    return true;
}

std::vector<AVFrame*> XDecode::End()
{
    std::vector<AVFrame*> res;
    unique_lock<mutex> lock(mux_);
    if (!c_) return res;

    // 1. 发送 NULL 告诉解码器没有更多数据
    int re = avcodec_send_packet(c_, NULL);
    if (re < 0) return res;

    // 2. 循环接收所有剩余帧
    while (true)
    {
        AVFrame* frame = av_frame_alloc();
        re = avcodec_receive_frame(c_, frame);

        if (re == 0)
        {
            // 成功取到一帧，保存
            res.push_back(frame);
        }
        else if (re == AVERROR_EOF)
        {
            // 解码器真正结束
            av_frame_free(&frame);
            break;
        }
        else if (re == AVERROR(EAGAIN))
        {
            // flush 时一般不会出现，但保险起见释放 frame
            av_frame_free(&frame);
            break;
        }
        else
        {
            // 其他错误
            av_frame_free(&frame);
            break;
        }
    }

    return res;
}
