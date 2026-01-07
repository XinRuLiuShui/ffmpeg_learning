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

#pragma once
/// <summary>
/// 封装和解封装基类
/// </summary>
#include <mutex>
struct AVFormatContext;
struct AVCodecParameters;
struct AVPacket;
struct AVCodecContext;
struct XRational
{
    int num; ///< Numerator
    int den; ///< Denominator
};
class XFormat
{
public:
    /// <summary>
    /// 复制参数 线程安全
    /// </summary>
    /// <param name="stream_index">对应c_->streams 下标</param>
    /// <param name="dst">输出参数</param>
    /// <returns>是否成功</returns>
    bool CopyPara(int stream_index, AVCodecParameters* dst);
    bool CopyPara(int stream_index, AVCodecContext* dts);

    /// <summary>
    /// 设置上下文，并且清理上次的设置的值，如果传递NULL，相当于关闭上下文3
    /// 线程安全
    /// </summary>
    /// <param name="c"></param>
    void set_c(AVFormatContext* c);
    int audio_index() { return audio_index_; }
    int video_index() { return video_index_; }
    XRational video_time_base(){ return video_time_base_; }
    XRational audio_time_base() { return audio_time_base_; }

    //根据timebase换算时间
    bool RescaleTime(AVPacket *pkt,long long offset_pts, XRational time_base);

    int video_codec_id() { return video_codec_id_; }
protected:
    AVFormatContext* c_ = nullptr;  //封装解封装上下文
    std::mutex mux_;                //c_ 资源互斥
    int video_index_ = 0;//video和audio在stream中索引
    int audio_index_ = 1;
    XRational video_time_base_ = {1,25};
    XRational audio_time_base_ = {1,9000};
    int video_codec_id_ = 0;       //编码器ID
};

