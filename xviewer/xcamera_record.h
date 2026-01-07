#pragma once
#include "xtools.h"
#include <string>
class XCameraRecord:public XThread
{
public:
	void set_rtsp_url(std::string url) { rtsp_url_ = url; }
	void set_save_path(std::string s) { save_path_ = s; }
	void set_save_file_sec(int s) { save_file_sec = s; }
private:
	void Main() override;
	long long save_file_sec = 5;	//多少秒创建一个新文件储存视频
	std::string rtsp_url_;
	std::string save_path_;
};

