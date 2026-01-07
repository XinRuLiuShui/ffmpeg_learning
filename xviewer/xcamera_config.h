#pragma once
#include <vector>
#include <mutex>
struct XCameraData
{
	char name[1024] = { 0 };
	char url[4096] = { 0 };	//摄像机主码流
	char sub_url[4096] = { 0 };	//摄像机辅码流
	char save_path[4096] = { 0 };	//视频录制存放目录
};

class XCameraConfig
{
public:
	//唯一对象实例,单件模式
	static XCameraConfig* Instance()
	{
		static XCameraConfig xc;
		return &xc;
	}

	void Push(const XCameraData& data);	//插入相机数据，线程的安全

	/// <summary>
	/// 获取摄像机
	/// </summary>
	/// <param name="index">摄像机索引从零开始</param>
	/// <returns>判断name是否为空，若为空，返回空对象</returns>
	XCameraData GetCam(int index);

	/// <summary>
	/// 修改指定摄像机数据
	/// </summary>
	/// <param name="index">摄像机索引</param>
	/// <param name="data">摄像机数据结构</param>
	/// <returns></returns>
	bool SetCam(int index, const XCameraData& data);

	/// <summary>
	/// 删除摄像机
	/// </summary>
	/// <param name="index">摄像机索引</param>
	/// <returns></returns>
	bool DelCam(int index);

	/// <summary>
	/// 返回相机数量
	/// </summary>
	/// <returns>若没有相机的话，返回零</returns>
	int GetCamCount();

	/// <summary>
	/// 存储配置文件
	/// </summary>
	/// <param name="path"></param>
	/// <returns></returns>
	bool Save(const char* path);

	/// <summary>
	/// 读取配置
	/// </summary>
	/// <param name="path"></param>
	/// <returns></returns>
	bool Load(const char* path);
private:
	XCameraConfig(){}	//构造私有,单件模式
	std::vector<XCameraData> cams_;
	std::mutex mtx_;
};

