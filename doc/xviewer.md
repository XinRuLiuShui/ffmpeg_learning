


> Written with [StackEdit](https://stackedit.io/).

## 存储日期与路径
``` cpp
struct XCamViedo
{
    QString filepath;
    QDateTime datetime;
};
static map<QDate, vector<XCamViedo>> cam_date_videos;
cam_date_videos;
```
map建立日期与录像的路径与具体时间的关系，方便后续查看回放视频时检索功能

## Qt界面
``` cpp
XViewer::XViewer(QWidget *parent)
    : QWidget(parent)
{
	//...
}
```

### 设置布局
``` cpp
//垂直布局器 head body
    auto vlay = new QVBoxLayout();
    //边框0边距
    vlay->setContentsMargins(0, 0, 0, 0);
    //元素之间的边距
    vlay->setSpacing(0);

    vlay->addWidget(ui.head);
    vlay->addWidget(ui.body);
    this->setLayout(vlay);
```

### 右键菜单功能
``` cpp
	auto m = left_menu_.addMenu(C("视图"));
    auto a = m->addAction(C("1窗口"));
    connect(a, SIGNAL(triggered()), this, SLOT(View1()));
    auto b = m->addAction(C("4窗口"));
    connect(b, SIGNAL(triggered()), this, SLOT(View4()));
    auto c = m->addAction(C("9窗口"));
    connect(c, SIGNAL(triggered()), this, SLOT(View9()));
    auto d = m->addAction(C("16窗口"));
    connect(d, SIGNAL(triggered()), this, SLOT(View16()));
    auto e = m->addAction(C("全部开始录制"));
    connect(e, SIGNAL(triggered()), this, SLOT(StartRecord()));
    auto f = m->addAction(C("全部停止录制"));
    connect(f, SIGNAL(triggered()), this, SLOT(StopRecord()));
    View9();
```
其中`left_menu_`定义在	`xviewer.h`中
``` cpp
class XViewer : public QWidget
{
    Q_OBJECT

public:
	...
	
private:
    Ui::XViewerClass ui;
    QMenu left_menu_;
};
```
``` cpp
void XViewer::contextMenuEvent(QContextMenuEvent* event)
{
    //鼠标右键显示菜单
    left_menu_.exec(QCursor::pos());
    event->accept();
}
```
### 载入配置文件，这里面存储了XCameraData
``` cpp
XCameraConfig::Instance()->Load(CAMS_CONFIG_FILE);
```
``` cpp

bool XCameraConfig::Load(const char* path)
{
	if (!path)
	{
		return false;
	}
	ifstream ifs(path, ios::binary);
	if (!ifs)
	{
		return false;
	}
	XCameraData data;
	cams_.clear();
	unique_lock<mutex>lock(mtx_);
	for (;;)
	{
		ifs.read((char*)&data, sizeof(data));
		if (ifs.gcount() != sizeof(data))
		{
			ifs.close();
			return true;
		}
		cams_.push_back(data);
	}
	return true;
}
```
``` cpp
#define CAMS_CONFIG_FILE "test.db"
```
将配置信息放入`cams_`中，方便读取，此时是读取数据，在后续刷新函数中显示
#### 配置信息的数据结构
``` cpp
struct XCameraData
{
	char name[1024] = { 0 }; //名称
	char url[4096] = { 0 };	//摄像机主码流
	char sub_url[4096] = { 0 };	//摄像机辅码流
	char save_path[4096] = { 0 };	//视频录制存放目录
};
```
#### 管理相机配置的类 XCameraConfig
``` cpp
class XCameraConfig
{
public:
	//唯一对象实例,单件模式
	static XCameraConfig* Instance()
	{
		static XCameraConfig xc;
		return &xc;
	}
	...
private:
	XCameraConfig(){}	//构造私有,单件模式
	std::vector<XCameraData> cams_;
	std::mutex mtx_;
};
```
XCameraConfig使用单例模式，可写入和读取相机配置信息。

### 刷新左侧相机
``` cpp
//刷新左侧相机列表
ReflashCams();
```
``` cpp
void XViewer::ReflashCams()
{
    auto xc = XCameraConfig::Instance();
    ui.cam_list->clear();
    int count = xc->GetCamCount();
    for (int i = 0; i < count; i++)
    {
        auto cam = xc->GetCam(i);
        auto item = new QListWidgetItem(QIcon(":/XViewer/img/cam.png"),C(cam.name));
        ui.cam_list->addItem(item);
    }
}
```
显示相机列表

`ui.cam_list`为Qt中的一个列表控件

### 开始渲染
``` cpp
//启动定时器渲染视频
startTimer(1);
```
``` cpp
void XViewer::timerEvent(QTimerEvent* ev)
{
    //窗口总数量
    int windows_num = sizeof(cam_windows) / sizeof(XCameraWidget*);
    for (int i = 0; i < windows_num; i++)
    {
        if (cam_windows[i])
        {
            cam_windows[i]->Draw();
        }
    }
}
```
``` cpp
static XCameraWidget* cam_windows[16] = { 0 };
```
此时的`cam_windows`数量由`View`函数控制

#### 自定义控件XCameraWidget 用于显示相机画面
``` cpp
class XCameraWidget :
    public QWidget
{
    //必须要加，否则无法使用槽函数
    Q_OBJECT
public:
    XCameraWidget(QWidget* p = nullptr);

    //拖拽进入
    void dragEnterEvent(QDragEnterEvent* ev) override;
    //拖拽松开
    void dropEvent(QDropEvent* ev) override;

    //渲染重载
    void paintEvent(QPaintEvent* ev);

    //打开播放视频
    bool Open(const char* url);

    void Draw();

    //清理资源
    ~XCameraWidget();
private:
    XDecodeTask* decode_task_ = nullptr;
    XDemuxTask* demux_task_ = nullptr;
    XVideoView* view_ = nullptr;
};
```
开启定时器，根据窗口数量进行渲染。

其中`XCameraWidget`为自定义的控件，用来显示相机画面，可以将左侧相机名称拖拽至右侧控件中，然后开始播放视频

### 相机设置
``` cpp

void XViewer::SetCam(int index)
{
    auto xc = XCameraConfig::Instance();

    QDialog dlg(this);
    dlg.resize(400, 150);
    QFormLayout lay;
    dlg.setLayout(&lay);

    QLineEdit name_edit;
    lay.addRow(C("名称"), &name_edit);

    QLineEdit url_edit;
    lay.addRow(C("主码流"), &url_edit);

    QLineEdit sub_url_edit;
    lay.addRow(C("辅码流"), &sub_url_edit);

    QLineEdit save_path_edit;
    lay.addRow(C("保存目录"), &save_path_edit);

    QPushButton save;
    save.setText(C("保存"));
    connect(&save, SIGNAL(clicked()), &dlg, SLOT(accept()));
    lay.addRow("", &save);

    //修改摄像机
    if (index >= 0)
    {
        auto cam = xc->GetCam(index);
        name_edit.setText(C(cam.name));
        url_edit.setText(C(cam.url));
        sub_url_edit.setText(C(cam.sub_url));
        save_path_edit.setText(C(cam.save_path));

    }

    for (;;)
    {
        if (dlg.exec() == QDialog::Accepted)
        {
            if (name_edit.text().isEmpty())
            {
                QMessageBox::information(0, "error", C("请输入名称"));
                continue;
            }

            if (url_edit.text().isEmpty())
            {
                QMessageBox::information(0, "error", C("请输入主码流"));
                continue;
            }

            if (sub_url_edit.text().isEmpty())
            {
                QMessageBox::information(0, "error", C("请输入辅码流"));
                continue;
            }

            if (save_path_edit.text().isEmpty())
            {
                QMessageBox::information(0, "error", C("请输入保存目录"));
                continue;
            }
            break;
        }
        return; //这里要写return，否则打开窗口之后点关闭,关闭不了
    }

    XCameraData data;
    strcpy_s(data.name, name_edit.text().toLocal8Bit());
    strcpy_s(data.url, url_edit.text().toLocal8Bit());
    strcpy_s(data.sub_url, sub_url_edit.text().toLocal8Bit());
    strcpy_s(data.save_path, save_path_edit.text().toLocal8Bit());

    //修改摄像机
    if (index >= 0)
    {
        xc->SetCam(index, data);
    }
    else
    {
        xc->Push(data);
    }
    xc->Save(CAMS_CONFIG_FILE);
    ReflashCams();
}

void XViewer::AddCam()
{
    SetCam(-1);
}

void XViewer::SetCam()
{
    int row = ui.cam_list->currentIndex().row();
    if (row < 0)
    {
        QMessageBox::information(this, "error", C("请选择摄像机"));
        return;
    }
    SetCam(row);
}

void XViewer::DelCam()
{
    int row = ui.cam_list->currentIndex().row();
    if (row < 0)
    {
        QMessageBox::information(this, "error", C("请选择摄像机"));
        return;
    }
    std::stringstream ss;
    ss << "您确认要删除摄像机"
        << ui.cam_list->currentItem()->text().toLocal8Bit().constData()
        << "吗?";
    if (QMessageBox::information(this,"confirm",C(ss.str().c_str()),QMessageBox::Yes,QMessageBox::Cancel) == QMessageBox::Yes)
    {
        qDebug() << "/////////////////////////";
        XCameraConfig::Instance()->DelCam(row);
        XCameraConfig::Instance()->Save(CAMS_CONFIG_FILE);
        ReflashCams();
    }
    //else
    //{
    //    return;
    //}
}
```
主要函数为`void  XViewer::SetCam(int  index)` 

其中主要功能还是由 `XCameraConfig`类对`std::vector<XCameraData>  cams_`的修改并写入配置文件中

其中row是读取左侧相机列表被选择的控件的索引

row = ui.cam_list->currentIndex().row();

以上增删改的函数均通过槽函数绑定到了对应的按钮(按钮在qt creator中放置的，槽函数绑定qt creator也是在中设置的)

``` cpp
//修改摄像机
if (index >= 0)
{
    xc->SetCam(index, data);
}
else
{
    xc->Push(data);
}
xc->Save(CAMS_CONFIG_FILE);
ReflashCams();
```


``` cpp
bool XCameraConfig::Save(const char* path)
{
	if (!path)
	{
		return false;
	}
	ofstream ofs(path,ios::binary);
	if (!ofs)
	{
		return false;
	}
	unique_lock<mutex>lock(mtx_);
	for (auto cam : cams_)
	{
		ofs.write((char*)&cam, sizeof(cam));
	}
	ofs.close();
	return true;
}
```
``` cpp
void XViewer::AddCam()
{
    SetCam(-1);
}

void XViewer::SetCam()
{
    int row = ui.cam_list->currentIndex().row();
    if (row < 0)
    {
        QMessageBox::information(this, "error", C("请选择摄像机"));
        return;
    }
    SetCam(row);
}
```


## XCameraWidget控件
``` cpp
// xcamera_widget.h
#pragma once
#include <QtWidgets/QWidget>

class XDecodeTask;
class XDemuxTask;
class XVideoView;

class XCameraWidget :
    public QWidget
{
    //必须要加，否则无法使用槽函数
    Q_OBJECT
public:
    XCameraWidget(QWidget* p = nullptr);

    //拖拽进入
    void dragEnterEvent(QDragEnterEvent* ev) override;
    //拖拽松开
    void dropEvent(QDropEvent* ev) override;

    //渲染重载
    void paintEvent(QPaintEvent* ev);

    //打开播放视频
    bool Open(const char* url);

    void Draw();

    //清理资源
    ~XCameraWidget();
private:
    XDecodeTask* decode_task_ = nullptr;
    XDemuxTask* demux_task_ = nullptr;
    XVideoView* view_ = nullptr;
};
```
### 拖拽功能
``` cpp
XCameraWidget::XCameraWidget(QWidget* p):QWidget(p)
{
	//允许接收拖拽
	this->setAcceptDrops(true);
}

void XCameraWidget::dragEnterEvent(QDragEnterEvent* ev)
{

	ev->acceptProposedAction();

}

void XCameraWidget::dropEvent(QDropEvent* ev)
{
	//qDebug() << ev->source()->objectName();
	auto wid = (QListWidget*)ev->source();
	qDebug() << wid->currentRow();
	auto cam = XCameraConfig::Instance()->GetCam(wid->currentRow());
	Open(cam.url);
}
```
其中左侧列表中的控件在qt creator中设置为了可拖拽，同时XCameraWidget设置为可接收拖拽，从而使XCameraWidget可以接收左侧拖进来的控件，进一步读取控件的数据，便可以得知相机信息，为播放视频做准备

`void XCameraWidget::dropEvent(QDropEvent* ev)`接收到后解析出视频url，之后调用`bool  Open(const  char*  url)`进行解封装和解码


``` cpp

bool XCameraWidget::Open(const char* url)
{
	if (demux_task_)
	{
		demux_task_->Stop();
		delete demux_task_;
		demux_task_ = nullptr;
	}
	if (decode_task_)
	{
		decode_task_->Stop();
		delete decode_task_;
		decode_task_ = nullptr;
	}
	if (view_)
	{
		view_->Close();
		delete view_;
		view_ = nullptr;
	}
	//创建解封装线程
	demux_task_ = new XDemuxTask();
	if (!demux_task_->Open(url))
	{
		return false;
	}

	//创建解码线程
	decode_task_ = new XDecodeTask();
	auto para = demux_task_->CopyVideoPara();
	if (!decode_task_->Open(para->para))
	{
		return false;
	}
	demux_task_->set_next(decode_task_);
	
	decode_task_->set_video_stream_index(demux_task_->video_index());
	//创建渲染器
	view_ = XVideoView::Creat(XVideoView::SDL);
	view_->set_win_id((void*)winId());
	view_->Init(para->para);

	//启动线程
	demux_task_->Start();
	decode_task_->Start();
	return true;
}
```
`view_->set_win_id((void*)winId());`设置渲染窗口为当前`XCameraWidget`实例化的窗口id，使渲染的结果显示到右侧的窗口中

---
``` cpp
void XViewer::timerEvent(QTimerEvent* ev)
{
    //窗口总数量
    int windows_num = sizeof(cam_windows) / sizeof(XCameraWidget*);
    for (int i = 0; i < windows_num; i++)
    {
        if (cam_windows[i])
        {
            cam_windows[i]->Draw();
        }
    }
}
```
``` cpp
void XCameraWidget::Draw()
{
	if (!demux_task_ || !decode_task_ || !view_)
	{
		return;
	}
	auto frame = decode_task_->GetFrame();
	if (!frame)
	{
		return;
	}
	view_->DrawFrame(frame);
	XFreeFrame(&frame);
}
```
渲染部分在定时器中进行


### 录制视频
``` cpp
void XViewer::StartRecord()
{
    StopRecord();
    qDebug() << C("全部摄像头开始录制");
    ui.status;
    ui.status->setText(C("录制中..."));
    auto xc = XCameraConfig::Instance();
    int count = xc->GetCamCount();
    for (int i = 0; i < count; i++)
    {
        auto cam = xc->GetCam(i);
        stringstream ss;
        ss << cam.save_path << "/" << i << "/";
        QDir dir;
        QString path = QString::fromStdString(ss.str());
        if (!dir.mkpath(path))
        {
            qDebug() << C("目录创建失败:") << path;
            continue;
        }
        XCameraRecord* record = new XCameraRecord();
        record->set_rtsp_url(cam.url);
        record->set_save_path(ss.str());
        record->set_save_file_sec(5);
        record->Start();
        records_vector.push_back(record);

    }
}

void XViewer::StopRecord()
{
    qDebug() << C("全部摄像头停止录制");
    ui.status->setText(C("监控中..."));
    for (auto rec:records_vector)
    {
        rec->Stop();
        delete rec;
        rec = nullptr;
    }
    records_vector.clear();
}
```

``` cpp
static vector<XCameraRecord*> records_vector;
```
`records_vector`是存放`XCameraRecord`类的容器

---
## XCameraRecord类

``` cpp
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
```
``` cpp
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
```
`class  XCameraRecord:public  XThread`

`XCameraRecord`类继承`XThread`，`XThread`在另一章xcodec库中解释

测试过程中以5秒为间隔进行视频录制

## 查看指定相机指定日期的回放视频
``` cpp

void XViewer::SelectCamera(QModelIndex index)
{
    qDebug() << "SelectCamera " << index.row();
    auto conf = XCameraConfig::Instance();
    auto cam = conf->GetCam(index.row());
    if (cam.name[0] == '\0')
    {
        return;
    }
    stringstream ss;
    ss << cam.save_path << "/" << index.row() << "/";

    QDir dir(C(ss.str().c_str()));
    if (!dir.exists())
    {
        return;
    }

    QStringList filters;
    filters << "*.mp4";
    dir.setNameFilters(filters);
    cam_date_videos.clear();
    ui.calendarWidget->ClearDate();

    auto files = dir.entryInfoList();
    for (auto file : files)
    {
        //"cam_2026_01_08_14_25_21.mp4"
        QString filename = file.fileName();
        
        auto temp = filename.left(filename.size() - 4);
        temp = temp.right(temp.length() - 4);
        QDateTime dateTime = QDateTime::fromString(temp, "yyyy_MM_dd_hh_mm_ss");
        //qDebug() << "DateTime:" << dateTime.toString();
        qDebug() << "DateTime:" << dateTime.date();


        ui.calendarWidget->AddDate(dateTime.date());

        // 创建 XCamViedo 对象
        XCamViedo video;
        video.filepath = file.absoluteFilePath();  // 存储文件路径
        video.datetime = dateTime;  // 存储对应的 QDateTime

        // 获取日期部分作为键
        QDate date = dateTime.date();

        // 将视频保存到 cam_date_videos 中
        cam_date_videos[date].push_back(video);  // 将视频添加到对应日期的 vector 中

        //qDebug() << file.fileName();
    }
    ui.calendarWidget->showNextMonth();
    ui.calendarWidget->showPreviousMonth();
}

void XViewer::SelectDate(QDate date)
{
    ui.timelist->clear();
    qDebug() << "SelectDate " << date.toString();

    // 查找日期对应的视频列表
    auto videos = cam_date_videos[date];

    // 如果该日期没有对应的视频，输出提示
    if (videos.empty()) {
        qDebug() << "No videos for the selected date.";
        return;
    }

    // 输出该日期对应的视频信息
    for (const auto& video : videos) {
        qDebug() << "Video Filepath:" << video.filepath;
        qDebug() << "Video DateTime:" << video.datetime.toString();

        // 显示时间，保持你的 videoInfo 格式
        QString videoInfo = video.datetime.time().toString();
        auto item = new QListWidgetItem(videoInfo);

        // 设置该项的自定义数据为视频的绝对路径
        item->setData(Qt::UserRole, video.filepath);

        // 将列表项添加到 QListWidget
        ui.timelist->addItem(item);
    }

}
```

	//xviewer.ui
    <connection>
       <sender>cam_list</sender>
       <signal>clicked(QModelIndex)</signal>
       <receiver>XViewerClass</receiver>
       <slot>SelectCamera(QModelIndex)</slot>
       <hints>
        <hint type="sourcelabel">
         <x>96</x>
         <y>191</y>
        </hint>
        <hint type="destinationlabel">
         <x>96</x>
         <y>-171</y>
        </hint>
       </hints>
    </connection>

`SelectCamera`绑定到了`cam_list`

### 自定义日历类
``` cpp
class XCalendar :
    public QCalendarWidget
{
public:
    XCalendar(QWidget* p);
    void paintCell(QPainter* painter, const QRect& rect, const QDate& date) const override;

    void AddDate(QDate d) { mdate_.insert(d); }
    void ClearDate() { mdate_.clear(); }

private:
    std::set<QDate> mdate_; //存放有视频的日期
};
```
`SelectCamera`函数根据当前选择的相机的`index`，去查找配置类`XCameraConfig`实例，获取当前相机录制视频的保存路径，用过滤器`filters`找到所有视频。然后遍历所有视频，解析出所有日期(视频在保存时名称有固定的格式)，将解析到的日期保存到日期类`XCalendar`的`mdate_`中
`std::set<QDate>  mdate_; //存放有视频的日期`

然后保存指定日期对应的所有视频
``` cpp
// 创建 XCamViedo 对象
XCamViedo video;
video.filepath = file.absoluteFilePath();  // 存储文件路径
video.datetime = dateTime;  // 存储对应的 QDateTime

// 获取日期部分作为键
QDate date = dateTime.date();

// 将视频保存到 cam_date_videos 中
cam_date_videos[date].push_back(video);  // 将视频添加到对应日期的 vector 中
```
``` cpp
struct XCamViedo
{
    QString filepath;
    QDateTime datetime;
};
static map<QDate, vector<XCamViedo>> cam_date_videos;
```

	
 

     <connection>
       <sender>calendarWidget</sender>
       <signal>clicked(QDate)</signal>
       <receiver>XViewerClass</receiver>
       <slot>SelectDate(QDate)</slot>
       <hints>
        <hint type="sourcelabel">
         <x>499</x>
         <y>204</y>
        </hint>
        <hint type="destinationlabel">
         <x>503</x>
         <y>-231</y>
        </hint>
       </hints>
      </connection>
`SelectDate`绑定到了日历类`calendarWidget`
`SelectDate`函数根据选择的日期去查看`cam_date_videos`，看是否有视频有，有的话创建列表控件添加到`timelist`中

``` cpp
// 输出该日期对应的视频信息
for (const auto& video : videos) {
  qDebug() << "Video Filepath:" << video.filepath;
  qDebug() << "Video DateTime:" << video.datetime.toString();

  // 显示时间，保持你的 videoInfo 格式
  QString videoInfo = video.datetime.time().toString();
  auto item = new QListWidgetItem(videoInfo);

  // 设置该项的自定义数据为视频的绝对路径
  item->setData(Qt::UserRole, video.filepath);

  // 将列表项添加到 QListWidget
  ui.timelist->addItem(item);
}
```
### 日历中有视频的日期特殊显示 方便用户查找
``` cpp
void XCalendar::paintCell(QPainter* painter, const QRect& rect, const QDate& date) const
{

    if (mdate_.find(date) == mdate_.end()) {
        QCalendarWidget::paintCell(painter, rect, date);
        return;
    }


    //设置字体
    auto font = painter->font();
    font.setPixelSize(36);
    painter->setFont(font);



    if (date.dayOfWeek() == Qt::Saturday || date.dayOfWeek() == Qt::Sunday) {
        painter->fillRect(rect, QColor("#FFDAB9"));
    }


    //将当天的日期用红色边框标记出来
    if (date == QDate::currentDate()) {
        painter->setPen(QPen(QColor("#7B68EE"), 2)); // 设置红色画笔，线宽2
        painter->drawRect(rect.adjusted(1, 1, -1, -1)); // 绘制矩形边框
    }
    if (date == selectedDate())
    {
        //painter->setBrush(QColor("#63B8FF"));
        //painter->drawRect(rect);
        painter->fillRect(rect, QColor("#63B8FF"));
    }

    painter->setPen(QPen(Qt::black, 2));
    painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));

}
```
---
``` cpp
if (mdate_.find(date) == mdate_.end()) {
    QCalendarWidget::paintCell(painter, rect, date);
    return;
}
```
如果没有在`mdate_`中的日期则交给默认的函数显示`QCalendarWidget::paintCell(painter, rect, date)`

## 回放视频
``` cpp
void XViewer::PlayVideo(QModelIndex index)
{
    qDebug() << "PlayVideo " << index.row();

    // 获取选中项的 QListWidgetItem
    QListWidgetItem* selectedItem = ui.timelist->currentItem();

    // 获取该项存储的绝对路径（Qt::UserRole 存储的数据）
    QString videoPath = selectedItem->data(Qt::UserRole).toString();

    qDebug() << "Selected Video Path:" << videoPath;
    static XPlayVideo play_video;
    play_video.Open(videoPath.toStdString().c_str());
    play_video.show();
}
```
``` cpp
class XPlayVideo : public QMainWindow
{
	Q_OBJECT

public:
	XPlayVideo(QWidget *parent = nullptr);
	~XPlayVideo();
	bool Open(const char* url);
	void timerEvent(QTimerEvent* ev) override;
	void closeEvent(QCloseEvent* ev) override;

	void Close();
private:
	Ui::XPlayVideoClass ui;
	XDecodeTask decode_task_;
	XDemuxTask demux_task_;
	XVideoView* view_ = nullptr;
};
```
`XPlayVideo`类有解封装和解码器
