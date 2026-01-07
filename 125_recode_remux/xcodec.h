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
#include <mutex>
#include <vector>
struct AVCodecContext;
struct AVPacket;
struct AVFrame;

void PrintErr(int err);

////////////////////////////////////////////
//// 编码和解码的基类
class XCodec
{
public:
    //////////////////////////////////////////
    /// 创建编解码上下文
    /// @para codec_id 编码器ID号，对应ffmpeg
    /// @return 编码上下文 ,失败返回nullptr
    static AVCodecContext* Create(int codec_id,bool is_encode);

    //////////////////////////////////////////
    /// 设置对象的编码器上下文 上下文传递到对象中，资源由XEncode维护
    /// 加锁 线程安全
    /// @para c 编码器上下文 如果c_不为nullptr，则先清理资源
    void set_c(AVCodecContext* c);

    /////////////////////////////////////////////
    /// 设置编码参数，线程安全
    bool SetOpt(const char* key, const char* val);
    bool SetOpt(const char* key, int val);

    //////////////////////////////////////////////////////////////
    /// 打开编码器 线程安全
    bool Open();

    ///////////////////////////////////////////////////////////////
    //根据AVCodecContext 创建一个AVFrame，需要调用者释放av_frame_free
    AVFrame* CreateFrame();


protected:
    AVCodecContext* c_ = nullptr;  //编码器上下文
    std::mutex mux_;               //编码器上下文锁
};

