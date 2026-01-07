#include <iostream>
using namespace std;

#include "xtools.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}
#include "xdemux.h"
#include "xmux.h"
#include "xcodec.h"
#include "xdecode.h"
#include "xencode.h"
#include "xvideo_view.h"

#include "xdemuxtask.h"
#include "xdecode_task.h"

//预处理指令导入库
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")//使用av_strerror需要添加这个库
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swscale.lib")//图像缩放库
class TestThread:public XThread
{
public:
	void Main()
	{
		LOGDEBUG("TestThread start");
		while (!is_exit_)
		{
			this_thread::sleep_for(1ms);
		}
		LOGDEBUG("TestThread end");
	}

private:

};

int main(int argc, char* argv[])
{

	string in_file = "rtsp://127.0.0.1:8554/test";
	XDemuxTask demux_task;
	
	cout << "正在连接RTSP流: " << in_file << endl;
	cout << "如果流还未启动，将每隔100ms重试..." << endl;
	
	for (;;)
	{
		if (demux_task.Open(in_file))
		{
			cout << "成功连接到RTSP流!" << endl;
			break;
		}
		cout << "连接失败，正在重试..." << endl;
		MSleep(100);
	}

	auto para = demux_task.CopyVideoPara();

	auto view = XVideoView::Creat(XVideoView::SDL);
	view->Init(para->para);

	XDecodeTask decode_task;
	if (!decode_task.Open(para->para))
	{
		LOGERROR("decode_task.Open failed!");
	}
	else
	{
		LOGINFO("decode_task.Open success");
		demux_task.set_next(&decode_task);
		demux_task.Start();
		decode_task.Start();
	}
	
	for (;;)
	{
		auto f = decode_task.GetFrame();
		view->DrawFrame(f);
		XFreeFrame(&f);
	}
	

	cout << "开始读取数据..." << endl;
	getchar();
	return 0;
}