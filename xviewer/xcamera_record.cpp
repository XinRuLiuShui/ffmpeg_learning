#include "xcamera_record.h"
#include "xdemux_task.h"
#include "xmux_task.h"

#include <iostream>
//获取系统当前时间
#include <chrono>
//时间格式化
#include <iomanip>
#include <sstream>

using namespace std;
using namespace chrono;

//生成存储的视频文件名
static std::string GetFileName(string path)
{
	stringstream ss;
	auto t = system_clock::to_time_t(system_clock::now());
	struct tm timeinfo;
	localtime_s(&timeinfo, &t);

	auto time_str = put_time(&timeinfo, "%Y_%m_%d_%H_%M_%S");
	ss << path << "/" << "cam_" << time_str << ".mp4";
	return ss.str();
}

void XCameraRecord::Main()
{
	XDemuxTask demux_task;
	XMuxTask mux_task;
	if (rtsp_url_.empty())
	{
		LOGERROR("打开 rtsp_url_ 错误！");
		return;
	}
	for (;;)
	{
		if (!is_exit_)
		{
			if (demux_task.Open(rtsp_url_))
			{
				cout << "成功连接到RTSP流!" << endl;
				break;
			}
			cout << "连接失败，正在重试..." << endl;
			MSleep(100);
		}
		else
		{
			return;
		}
	}
	auto vpara = demux_task.CopyVideoPara();
	if (!vpara)
	{
		//需要考虑资源释放问题
		demux_task.Stop();
		return;
	}
	//提前启动解封装，防止超时
	demux_task.Start();
	auto apara = demux_task.CopyAudioPara();
	AVCodecParameters* para = nullptr;
	AVRational* timabase = nullptr;
	if (apara)
	{
		para = apara->para;
		timabase = apara->time_base;
	}
	if (!mux_task.Open(GetFileName(save_path_).c_str(), vpara->para, vpara->time_base, para, timabase))
	{
		demux_task.Stop();
		mux_task.Stop();
		LOGERROR("mux_task.Open failed!");
		return;
	}
	auto a = demux_task.video_index();
	auto b = demux_task.audio_index();
	demux_task.set_next(&mux_task);
	mux_task.set_video_index(demux_task.video_index());
	mux_task.set_audio_index(demux_task.audio_index());
	//mux_task.set_audio_index_(demux_task.CopyAudioPara)

	mux_task.Start();
	auto begin_time = NowMs();
	while (!is_exit_)
	{
		auto a = NowMs();
		if (NowMs() - begin_time >= (save_file_sec * 1000))
		{
			begin_time = NowMs();
			mux_task.Stop();
			if (!mux_task.Open(GetFileName(save_path_).c_str(), vpara->para, vpara->time_base, para, timabase))
			{
				demux_task.Stop();
				mux_task.Stop();
				LOGERROR("mux_task.Open failed!");
				return;
			}
			mux_task.set_video_index(demux_task.video_index());
			mux_task.set_audio_index(demux_task.audio_index());
			mux_task.Start();
		}
		MSleep(10);
	}
	

	mux_task.Stop();
	demux_task.Stop();
}
