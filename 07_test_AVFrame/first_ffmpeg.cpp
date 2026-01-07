#include <iostream>
using namespace std;

extern "C" {
	#include <libavcodec/avcodec.h>
}

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
int main(int argc, char* argv[])
{
	cout << avcodec_configuration() << endl;

	//创建frame对象
	AVFrame* frame1 = av_frame_alloc();
	AVFrame* frame2 = av_frame_alloc();

	//图像参数
	frame1->width = 400;
	frame1->height = 300;
	frame1->format = AV_PIX_FMT_ARGB;

	//分配空间 16字节对齐
	int re = av_frame_get_buffer(frame1, 16);
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf));
		cout << buf << endl;
	}
	cout << frame1->linesize[0] << endl;

	if (frame1->buf[0])
	{
		cout << frame1->buf[0] << endl;
		cout << av_buffer_get_ref_count(frame1->buf[0]) << endl; //线程安全
	}

	av_frame_ref(frame2, frame1);
	//frame1和frame2共同使用同一块buf空间
	//after ref
	//frame1
	//015A29C0
	//2
	//frame2
	//015A2F40
	//2
	cout << "after ref" << endl;
	cout << "frame1" << endl;
	if (frame1->buf[0])
	{
		cout << frame1->buf[0] << endl;
		cout << av_buffer_get_ref_count(frame1->buf[0]) << endl; //线程安全
	}
	cout << "frame2" << endl;
	if (frame2->buf[0])
	{
		cout << frame2->buf[0] << endl;
		cout << av_buffer_get_ref_count(frame2->buf[0]) << endl; //线程安全
	}
	av_frame_unref(frame2);
	cout << "after unref frame2" << endl;
	cout << "frame1" << endl;
	if (frame1->buf[0])
	{
		cout << frame1->buf[0] << endl;
		cout << av_buffer_get_ref_count(frame1->buf[0]) << endl; //线程安全
	}
	cout << "frame2" << endl;
	if (frame2->buf[0])
	{
		cout << frame2->buf[0] << endl;
		cout << av_buffer_get_ref_count(frame2->buf[0]) << endl; //线程安全
	}
	//引用计数为1，减1直接删除buf空间
	av_frame_unref(frame1);
	cout << "after unref frame1" << endl;
	cout << "frame1" << endl;
	if (frame1->buf[0])
	{
		cout << frame1->buf[0] << endl;
		cout << av_buffer_get_ref_count(frame1->buf[0]) << endl; //线程安全
	}
	cout << "frame2" << endl;
	if (frame2->buf[0])
	{
		cout << frame2->buf[0] << endl;
		cout << av_buffer_get_ref_count(frame2->buf[0]) << endl; //线程安全
	}
	//释放空间
	av_frame_free(&frame1);
	av_frame_free(&frame2);
}