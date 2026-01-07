#include <iostream>
#include <fstream>
using namespace std;

extern "C" {
#include <libavcodec/avcodec.h>
#include<libswscale/swscale.h>
}

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")

#pragma comment(lib,"swscale.lib")

#define YUV_FILE "400_300_25.yuv"
#define RGBA_FILE "800_600_25.rgba"
#define RGBA2YUV_FILE "400_300_25rgba2yuv.yuv"
int main(int argc, char* argv[])
{
	int width = 400;
	int height = 300;
	int rgb_width = 800;
	int rgb_height = 600;
	unsigned char* yuv[3] = { 0 };
	int yuv_linesize[3] = { width,width / 2,width / 2 };
	yuv[0] = new unsigned char[width * height];
	yuv[1] = new unsigned char[width * height / 4];
	yuv[2] = new unsigned char[width * height / 4];

	
	unsigned char* rgba = new unsigned char[rgb_width * rgb_height * 4];

	int rgba_linesize = rgb_width * 4;

	ifstream ifs;
	ifs.open(YUV_FILE, ios::binary);
	if (!ifs)
	{
		cerr << "open" << YUV_FILE << "failed!" << endl;
	}
	else {
		cout << "open success!" << endl;
	}

	ofstream ofs;
	ofs.open(RGBA_FILE, ios::binary);
	if (!ofs)
	{
		cerr << "open" << RGBA_FILE << "failed!" << endl;
	}
	else {
		cout << "open success!" << endl;
	}


	SwsContext* yuv2rgb = nullptr;
	for (int i = 0;i < 10;i++)
	{
		//读取YUV帧
		ifs.read((char*)yuv[0], width * height);
		ifs.read((char*)yuv[1], width * height / 4);
		ifs.read((char*)yuv[2], width * height / 4);
		if (ifs.gcount() == 0)
		{
			break;
		}

		//YUV420P转RGBA
		yuv2rgb = sws_getCachedContext(
			yuv2rgb,				//转换上下文，NULL则新建，若一致则使用，不一致先清空再创建
			width, height,
			AV_PIX_FMT_YUV420P,		//源格式
			rgb_width, rgb_height,
			AV_PIX_FMT_RGBA,		//目的格式
			SWS_BILINEAR,			//算法
			0, 0, 0					//过滤器参数
		);

		if (!yuv2rgb)
		{
			cerr << "sws_getCachedContext failed!" << endl;
			return -1;
		}


		uint8_t* data[1] = { 0 };
		data[0] = rgba;

		int lines[1] = { rgba_linesize };
		int re = sws_scale(
			yuv2rgb, yuv, yuv_linesize,
			0,
			height,
			data,
			lines
		);
		cout << re << " " << flush;
		ofs.write((char*)rgba, rgb_width * rgb_height * 4);
	}

	ofs.close();
	ifs.close();

	//RGBA转YUV420P
	ifs.open(RGBA_FILE, ios::binary);
	if (!ifs)
	{
		cerr << "open" << RGBA_FILE << "failed!" << endl;
	}
	else {
		cout << "open success!" << endl;
	}

	ofs.open(RGBA2YUV_FILE, ios::binary);
	if (!ofs)
	{
		cerr << "open" << RGBA2YUV_FILE << "failed!" << endl;
	}
	else {
		cout << "open success!" << endl;
	}

	SwsContext* rgb2yuv = nullptr;
	for (int i = 0; i < 10; i++)
	{
		//读取RGBA帧
		ifs.read((char*)rgba, rgb_width * rgb_height * 4);
		if (ifs.gcount() == 0)
		{
			break;
		}

		//RGBA转YUV420P
		rgb2yuv = sws_getCachedContext(
			rgb2yuv,				//转换上下文，NULL则新建，若一致则使用，不一致先清空再创建
			rgb_width, rgb_height,
			AV_PIX_FMT_RGBA,		//源格式
			width, height,
			AV_PIX_FMT_YUV420P,		//目的格式
			SWS_BILINEAR,			//算法
			0, 0, 0					//过滤器参数
		);

		if (!rgb2yuv)
		{
			cerr << "rgb2yuv sws_getCachedContext failed!" << endl;
			return -1;
		}


		uint8_t* data[1] = { 0 };
		data[0] = rgba;

		int lines[1] = { rgba_linesize };
		int re = sws_scale(
			rgb2yuv, data, lines,
			0,
			rgb_height,
			yuv,
			yuv_linesize
		);
		cout << re << " " << flush;
		//ofs.write((char*)yuv, width * height * 1.5);
		ofs.write((char*)yuv[0], width* height);
		ofs.write((char*)yuv[1], width* height / 4);
		ofs.write((char*)yuv[2], width* height / 4);
	}


	delete yuv[0];
	delete yuv[1];
	delete yuv[2];

	delete rgba;
	ofs.close();
	ifs.close();

	
}