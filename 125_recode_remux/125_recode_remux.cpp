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

#include <iostream>
#include <thread>
#include "xdemux.h"
#include "xmux.h"
#include "xdecode.h"
#include "xencode.h"
#include "xvideo_view.h"
using namespace std;
extern "C"
{ //指定函数是c语言函数，函数名不包含重载标注
//引用ffmpeg头文件
#include <libavformat/avformat.h>
}
////预处理指令导入库
//#pragma comment(lib,"avformat.lib")
//#pragma comment(lib,"avutil.lib")
//#pragma comment(lib,"avcodec.lib")
//void PrintErr(int err)
//{
//    char buf[1024] = { 0 };
//    av_strerror(err, buf, sizeof(buf) - 1);
//    cerr << endl;
//}
//#define CERR(err) if(err!=0){ PrintErr(err);getchar();return -1;}

int main(int argc, char* argv[])
{

    /// 输入参数处理
    //使用说明
    string useage = "124_test_xformat 输入文件 输出文件 开始时间（秒） 结束时间（秒） 视频宽 视频高\n";
    useage += "124_test_xformat v1080.mp4 test_out.mp4 10 20 400 300";
    cout << useage << endl;

    if (argc < 3)
    {
        return -1;
    }

    string in_file = argv[1];
    string out_file = argv[2];
    ////////////////////////////////////////////////////////////////////////////////////
    /// 截取10 ~ 20 秒之间的音频视频 取多不取少
    // 假定 9 11秒有关键帧 我们取第9秒
    int begin_sec = 0;    //截取开始时间
    int end_sec = 0;      //截取结束时间
    if (argc > 3)
        begin_sec = atoi(argv[3]);
    if (argc > 4)
        end_sec = atoi(argv[4]);

    int video_width = 0;
    int video_height = 0;
    if (argc > 6)
        video_width = atoi(argv[5]);
    video_height = atoi(argv[6]);


    ///////////////////////////////////////////////////////////////////////////






    //打开媒体文件
    //const char* url = "v1080.mp4";
    ////////////////////////////////////////////////////////////////////////////////////
    /// 解封装
    //解封装输入上下文

    XDemux demux;
    auto demux_c = demux.Open(in_file.c_str());

    demux.set_c(demux_c);

    ////////////////////////////////////////////////////////////////////////////////////

    


    long long video_begin_pts = 0;
    long long audio_begin_pts = 0;  //音频的开始时间
    long long video_end_pts = 0;
    //开始截断秒数 算出输入视频的pts
    //if (begin_sec > 0)
    {
        //计算视频的开始和结束播放pts 
        if (demux.video_index() >= 0 && demux.video_time_base().num > 0)
        {
            double t = (double)demux.video_time_base().den / (double)demux.video_time_base().num;
            video_begin_pts = t * begin_sec;
            video_end_pts = t * end_sec;
            demux.Seek(video_begin_pts, demux.video_index()); //移动到开始帧
        }

        //计算音频的开始播放pts
        if (demux.audio_index() >= 0 && demux.audio_time_base().num > 0)
        {
            double t = (double)demux.audio_time_base().den / (double)demux.audio_time_base().num;
            audio_begin_pts = t * begin_sec;
        }

    }

    /////////////////////////////////////////////////////////////////////////
    //// 视频解码的初始化
    XDecode decode;
    auto decode_c = decode.Create(demux.video_codec_id(), false);
    //设置视频解码器参数
    demux.CopyPara(demux.video_index(), decode_c);

    decode.set_c(decode_c);
    decode.Open();
    auto frame = decode.CreateFrame(); //解码后存储
    /////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////
    //// 视频编码的初始化

    if (demux.video_index() >= 0)
    {
        if (video_width <= 0)
            video_width = demux_c->streams[demux.video_index()]->codecpar->width;
        if (video_height <= 0)
            video_height = demux_c->streams[demux.video_index()]->codecpar->height;
    }
    XEncode encode;
    auto encode_c = encode.Create(AV_CODEC_ID_H265, true);
    encode_c->pix_fmt = AV_PIX_FMT_YUV420P;
    encode_c->width = video_width;
    encode_c->height = video_height;
    encode.set_c(encode_c);
    encode.Open();

    /////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////////////
    /// 封装
    //编码器上下文
    //const char* out_url = "test_mux.mp4";

    XMux mux;
    auto mux_c = mux.Open(out_file.c_str());
    mux.set_c(mux_c);
    auto mvs = mux_c->streams[mux.video_index()]; //视频流信息
    auto mas = mux_c->streams[mux.audio_index()]; //视频流信息

    //有视频
    if (demux.video_index() >= 0)
    {
        mvs->time_base.num = demux.video_time_base().num;
        mvs->time_base.den = demux.video_time_base().den;

        //复制视频参数
        //demux.CopyPara(demux.video_index(), mvs->codecpar);
        // 复制编码器格式
        avcodec_parameters_from_context(mvs->codecpar, encode_c);

    }
    //有音频
    if (demux.audio_index() >= 0)
    {
        mas->time_base.num = demux.audio_time_base().num;
        mas->time_base.den = demux.audio_time_base().den;
        //复制音频参数
        demux.CopyPara(demux.audio_index(), mas->codecpar);
    }

    // 写入头部，会改变timebase
    mux.WriteHead();

    ////////////////////////////////////////////////////////////////////////////////////

    int audio_count = 0;
    int video_count = 0;
    double total_sec = 0;
    AVPacket pkt;
    for (;;)
    {
        if (!demux.Read(&pkt))
        {
            break;
        }

        // 视频 时间大于结束时间
        if (video_end_pts > 0
            && pkt.stream_index == demux.video_index()
            && pkt.pts > video_end_pts)
        {
            av_packet_unref(&pkt);
            break;
        }

        if (pkt.stream_index == demux.video_index()) //视频
        {
            mux.RescaleTime(&pkt, video_begin_pts, demux.video_time_base());

            //解码视频
            if (decode.Send(&pkt))
            {
                while (decode.Recv(frame))
                {
                    // 修改图像尺寸 
                    //视频编码
                    auto epkt = encode.Encode(frame);
                    if (epkt)
                    {
                        epkt->stream_index = mux.video_index();
                        //写入视频帧 会清理pkt
                        mux.Write(epkt);
                        //av_packet_free(&epkt);
                    }
                }
            }

            video_count++;
            if (demux.video_time_base().den > 0)
                total_sec += pkt.duration * ((double)demux.video_time_base().num / (double)demux.video_time_base().den);
            av_packet_unref(&pkt);
        }
        else if (pkt.stream_index == demux.audio_index())
        {
            mux.RescaleTime(&pkt, audio_begin_pts, demux.audio_time_base());
            audio_count++;
            //写入音频帧 会清理pkt
            mux.Write(&pkt);
        }
        else
        {
            av_packet_unref(&pkt);
        }


    }

    //写入结尾 包含文件偏移索引
    mux.WriteEnd();
    demux.set_c(nullptr);
    mux.set_c(nullptr);
    encode.set_c(nullptr);
    cout << "输出文件" << out_file << ":" << endl;
    cout << "视频帧:" << video_count << endl;
    cout << "音频帧:" << audio_count << endl;
    cout << "总时长:" << total_sec << endl;

    return 0;
}

