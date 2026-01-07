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

#include "xmux.h"

#include <iostream>
#include <thread>
using namespace std;
extern "C" { //指定函数是c语言函数，函数名不包含重载标注
//引用ffmpeg头文件
#include <libavformat/avformat.h>
}
void PrintErr(int err);

#define BERR(err) if(err!= 0){PrintErr(err);return 0;}

//////////////////////////////////////////////////
//// 打开封装
AVFormatContext* XMux::Open(const char* url)
{
    AVFormatContext* c = nullptr;
    //创建上下文
    auto re = avformat_alloc_output_context2(&c, NULL, NULL, url);
    BERR(re);

    //添加视频音频流
    auto vs = avformat_new_stream(c, NULL);   //视频流
    vs->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    auto as = avformat_new_stream(c, NULL);   //音频流
    as->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;


    //打开IO
    re = avio_open(&c->pb, url, AVIO_FLAG_WRITE);
    BERR(re);
    return c;
}
bool XMux::Write(AVPacket* pkt)
{
    unique_lock<mutex> lock(mux_);
    if (!c_)return false;
    //写入一帧数据，内部缓冲排序dts，通过pkt=null 可以写入缓冲
    auto re = av_interleaved_write_frame(c_,pkt);
    BERR(re);
    return true;
}

bool XMux::WriteEnd()
{
    unique_lock<mutex> lock(mux_);
    if (!c_)return false;
    int re = 0;
    //auto re = av_interleaved_write_frame(c_, nullptr);//写入排序缓冲
    //BERR(re);
    re = av_write_trailer(c_);
    BERR(re);
    return true;
}
bool XMux::WriteHead()
{
    unique_lock<mutex> lock(mux_);
    if (!c_)return false;
    //会改变timebase
    auto re = avformat_write_header(c_, nullptr);
    BERR(re);

    //打印输出上下文
    av_dump_format(c_, 0, c_->url, 1);

    return true;
}