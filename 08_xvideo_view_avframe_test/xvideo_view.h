#pragma once
#include <mutex>
struct AVFrame;

// 视频渲染接口
// 隐藏SDL
// 线程安全
class XVideoView
{
public:
	enum Format
	{
		RGBA = 0,
		ARGB,
		YUV420P
	};

	enum RenderType
	{
		SDL = 0
	};
	static XVideoView* Creat(RenderType type);

	// 初始化渲染窗口 线程安全 多次调用
	// @para w 窗口宽度
	// @para h 窗口高度
	// @para fmt 像素格式
	// @para win_id 窗口句柄，若为空，创建新窗口
	// @return 是否创建成功
	virtual bool Init(int w, int h, Format fmt = RGBA, void* win_id = nullptr) = 0;

	//清理资源
	virtual void Close() = 0;

	//处理退出事件
	virtual bool IsExit() = 0;

	// 渲染图像 线程安全
	// @para data 渲染的二进制数据
	// @para linesize 一行数据的字节数，若<=0则根据像素格式自动算出大小
	// @return 是否渲染成功
	virtual bool Draw(const unsigned char* data, int linesize = 0) = 0;

	virtual bool Draw(
		const unsigned char* y, int y_pitch,
		const unsigned char* u, int u_pitch,
		const unsigned char* v, int v_pitch
	) = 0;
	void SetScale(int w, int h)
	{
		scale_w_ = w;
		scale_h_ = h;
	}

	bool DrawFrame(AVFrame* frame);
protected:
	int width_ = 0;		//材质宽度
	int height_ = 0;	//材质高度
	Format fmt_ = RGBA;	//像素格式
	std::mutex mtx_;	//确保线程安全
	
	int scale_w_ = 0;//显示大小
	int scale_h_ = 0;
};

