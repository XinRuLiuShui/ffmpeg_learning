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

#include "xdecode.h"
#include <iostream>
using namespace std;
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}
bool XDecode::Send(const AVPacket* pkt)  //发送解码
{
    unique_lock<mutex> lock(mux_);
    if (!c_)return false;
    auto re = avcodec_send_packet(c_, pkt);
    if (re != 0)return false;
    return true;
}

bool XDecode::Recv(AVFrame* frame)      //获取解码
{
    unique_lock<mutex> lock(mux_);
    if (!c_)return false;
    auto f = frame;
    if (c_->hw_device_ctx) //硬件加速
    {
        f = av_frame_alloc();
    }
    auto re = avcodec_receive_frame(c_, f);
    if (re == 0)
    {
        if (c_->hw_device_ctx) //GPU解码
        {
            //显存转内存 GPU =》 CPU
            re = av_hwframe_transfer_data(frame, f, 0); 
            av_frame_free(&f);
            if (re != 0)
            {
                PrintErr(re);
                return false;
            }
        }
        return true;
    }
    if (c_->hw_device_ctx)
        av_frame_free(&f);
    return false;
}
bool XDecode::InitHW(int type)
{
    unique_lock<mutex> lock(mux_);
    if (!c_)return false;
    ;
    AVBufferRef* ctx = nullptr; //硬件加速上下文
    auto re = av_hwdevice_ctx_create(&ctx, (AVHWDeviceType)type,NULL,NULL,0);
    if (re != 0)
    {
        PrintErr(re);
        return false;
    }
    c_->hw_device_ctx = ctx;
    cout << "硬件加速：" << type << endl;
    return true;
}
std::vector<AVFrame*> XDecode::End()    //获取缓存
{
    std::vector<AVFrame*> res;
    unique_lock<mutex> lock(mux_);
    if (!c_)return res;

    ///取出缓存数据
    int ret = avcodec_send_packet(c_, NULL);
    while (ret >= 0)
    {
        auto frame = av_frame_alloc();
        ret = avcodec_receive_frame(c_, frame);
        if (ret < 0)
        {
            av_frame_free(&frame);
            break;
        }
        res.push_back(frame);
    }
    return res;
}