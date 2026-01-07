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


#include "xvideo_view.h"
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class XSDL :public XVideoView
{
public:
    void Close() override;
    ////////////////////////////////////////////////
    /// 初始化渲染窗口 线程安全
    /// @para w 窗口宽度
    /// @para h 窗口高度
    /// @para fmt 绘制的像素格式
    /// @para win_id 窗口句柄，如果为空，创建新窗口
    /// @return 是否创建成功
    bool Init(int w, int h,
        Format fmt = RGBA) override;

    //////////////////////////////////////////////////
    /// 渲染图像 线程安全
    ///@para data 渲染的二进制数据
    ///@para linesize 一行数据的字节数，对于YUV420P就是Y一行字节数
    /// linesize<=0 就根据宽度和像素格式自动算出大小
    /// @return 渲染是否成功
    bool Draw(const unsigned  char* data,
        int linesize = 0) override;
    bool Draw(
        const unsigned  char* y, int y_pitch,
        const unsigned  char* u, int u_pitch,
        const unsigned  char* v, int v_pitch
    ) override;
    bool IsExit() override;
private:
    SDL_Window* win_ = nullptr;
    SDL_Renderer* render_ = nullptr;
    SDL_Texture* texture_ = nullptr;
};