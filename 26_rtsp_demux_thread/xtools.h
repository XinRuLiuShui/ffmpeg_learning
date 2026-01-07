#pragma once
#include <thread>
#include <mutex>

struct AVPacket;
enum XLogLevel
{
	XLOG_TYPE_DEBUG,
	XLOG_TYPE_INFO,
	XLOG_TYPE_ERROR,
	XLOG_TYPE_FATAL
};
#define LOG_MIN_LEVEL XLOG_TYPE_DEBUG
#define XLOG(s,level) \
	if(level >= LOG_MIN_LEVEL)	\
		std::cout<<level<<":"<<__FILE__<<":"<<__LINE__<<":"	\
		<<s<<std::endl;

#define LOGDEBUG(msg)  XLOG(msg, XLOG_TYPE_DEBUG)
#define LOGINFO(msg)   XLOG(msg, XLOG_TYPE_INFO)
#define LOGERROR(msg)  XLOG(msg, XLOG_TYPE_ERROR)
#define LOGFATAL(msg)  XLOG(msg, XLOG_TYPE_FATAL)

void MSleep(unsigned int ms);

//获取当前毫秒级时间戳 
long long NowMs();

class XThread
{
public:
	//启动线程
	virtual void Start();
	//停止线程(设置退出标志，等待线程退出)
	virtual void Stop();

	//设置责任链下一个节点，线程安全
	void set_next(XThread* xt)
	{
		std::unique_lock<std::mutex> lock(mtx_);
		next_ = xt;
	}

	//执行责任
	//注意这里不要加分号，要有一个空的花括号
	virtual void Do(AVPacket* pkt){}

	//传递到下一个责任链函数
	virtual void Next(AVPacket* pkt)
	{
		std::unique_lock<std::mutex> lock(mtx_);
		if (next_)
		{
			next_->Do(pkt);
		}
	}

	

protected:
	//线程入口函数
	virtual void Main() = 0;

	//标志线程退出
	bool is_exit_ = false;
	

	int index_ = 0;

private:
	std::thread th_;
	std::mutex mtx_;
	XThread* next_ = nullptr;//责任链的下一个节点
};

class XTools
{
};

