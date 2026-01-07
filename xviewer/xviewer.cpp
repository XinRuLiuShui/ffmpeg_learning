#include "xviewer.h"
#include "xcamera_config.h"
#include "xcamera_widget.h"

#include <sstream>
//鼠标事件
#include <QMouseEvent>
//垂直布局器
#include <QVBoxLayout>
//水平布局器
#include <QHBoxLayout>
//窗口大小发生变化
#include <QResizeEvent>
//右键菜单
#include <QContextMenuEvent>
//相机布局
#include <QGridLayout>
//获取用户输入
#include <QDialog>
//布局器
#include <QFormLayout>
//输入框
#include <QLineEdit>
//消息框
#include <QMessageBox>
//定时器
#include <QMouseEvent>

#include <QDebug>


//解决中文乱码
#define C(s) QString::fromLocal8Bit(s)

#define CAMS_CONFIG_FILE "test.db"

static XCameraWidget* cam_windows[16] = { 0 };

XViewer::XViewer(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    //去除原窗口边框
    setWindowFlags(Qt::FramelessWindowHint);

    //垂直布局器 head body
    auto vlay = new QVBoxLayout();
    //边框0边距
    vlay->setContentsMargins(0, 0, 0, 0);
    //元素之间的边距
    vlay->setSpacing(0);

    vlay->addWidget(ui.head);
    vlay->addWidget(ui.body);
    this->setLayout(vlay);

    //相机列表和预览窗口
    auto hlay = new QHBoxLayout();
    ui.body->setLayout(hlay);
    //边框0边距
    hlay->setContentsMargins(0, 0, 0, 0);

    //增加这行代码，否则会重叠在一起
    //元素之间的边距
    //hlay->setSpacing(0);

    hlay->addWidget(ui.left);
    hlay->addWidget(ui.cams);


    



    auto m = left_menu_.addMenu(C("视图"));
    auto a = m->addAction(C("1窗口"));
    connect(a, SIGNAL(triggered()), this, SLOT(View1()));
    auto b = m->addAction(C("4窗口"));
    connect(b, SIGNAL(triggered()), this, SLOT(View4()));
    auto c = m->addAction(C("9窗口"));
    connect(c, SIGNAL(triggered()), this, SLOT(View9()));
    auto d = m->addAction(C("16窗口"));
    connect(d, SIGNAL(triggered()), this, SLOT(View16()));

    View9();

    XCameraConfig::Instance()->Load(CAMS_CONFIG_FILE);
    //刷新左侧相机列表
    ReflashCams();
    

    //启动定时器渲染视频
    startTimer(1);
}

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


XViewer::~XViewer()
{}


static bool mouse_press = false;
static QPoint mouse_point;

void XViewer::mouseMoveEvent(QMouseEvent * ev)
{
    if (!mouse_press)
    {
        QWidget::mouseMoveEvent(ev);
        return;
    }
    this->move(ev->globalPos() - mouse_point);
}

void XViewer::mousePressEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        mouse_press = true;
        mouse_point = ev->pos();
    }
}

void XViewer::mouseReleaseEvent(QMouseEvent* ev)
{
}



void XViewer::MaxWindow()
{
    ui.max->setVisible(false);
    ui.normal->setVisible(true);
    showMaximized();
}



void XViewer::NormalWindow()
{
    ui.max->setVisible(true);
    ui.normal->setVisible(false);
    showNormal();
}

void XViewer::resizeEvent(QResizeEvent* ev)
{
    int x = width() - ui.head_button->width();
    int y = ui.head_button->y();
    ui.head_button->move(x, y);
}

void XViewer::contextMenuEvent(QContextMenuEvent* event)
{
    //鼠标右键显示菜单
    left_menu_.exec(QCursor::pos());
    event->accept();
}

void XViewer::View(int count)
{
    qDebug() << "View" << count;
    //1x1 2x2 3x3 4x4
    
    //确定列数
    int cols = sqrt(count);
    //窗口总数量
    int windows_num = sizeof(cam_windows) / sizeof(XCameraWidget*);

    auto lay = (QGridLayout*)ui.cams->layout();
    if (!lay)
    {
        lay = new QGridLayout();
        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(2);
        ui.cams->setLayout(lay);
    }
    //初始化窗口
    for (int i = 0; i < count; i++)
    {
        if (!cam_windows[i])
        {
            cam_windows[i] = new XCameraWidget();
            cam_windows[i]->setStyleSheet("background-color: rgb(81, 81, 81)");
        }
        int row = i / cols;
        int col = i % cols;
        lay->addWidget(cam_windows[i], row,col);
    }

    for (int i = count; i < windows_num; i++)
    {
        delete cam_windows[i];
        cam_windows[i] = nullptr;
    }
}


void XViewer::View1()
{
    View(1);
}

void XViewer::View4()
{
    View(4);
}

void XViewer::View9()
{ 
    View(9);
}

void XViewer::View16()
{
    View(16);
}

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

