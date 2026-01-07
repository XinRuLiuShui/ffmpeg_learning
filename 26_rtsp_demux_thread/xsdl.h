#pragma once
#include "xvideo_view.h"
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class XSDL :
    public XVideoView
{
	// 初始化渲染窗口 线程安全
	// @para w 窗口宽度
	// @para h 窗口高度
	// @para fmt 像素格式
	// @para win_id 窗口句柄，若为空，创建新窗口
	// @return 是否创建成功
	bool Init(int w, int h, Format fmt = RGBA) override;

	//清理资源
	void Close() override;

	//处理退出事件
	bool IsExit() override;

	// 渲染图像 线程安全
	// @para data 渲染的二进制数据
	// @para linesize 一行数据的字节数，若<=0则根据像素格式自动算出大小
	// @return 是否渲染成功
	bool Draw(const unsigned char* data, int linesize = 0) override;
	bool Draw(
		const unsigned char* y, int y_pitch,
		const unsigned char* u, int u_pitch,
		const unsigned char* v, int v_pitch
	) override;
	bool Draw(const unsigned char* y, int y_pitch,
		const unsigned char* uv, int uv_pitch
	) override;
private:
	SDL_Window* win_ = nullptr;
	SDL_Renderer* render_ = nullptr;
	SDL_Texture* texture_ = nullptr;
};

