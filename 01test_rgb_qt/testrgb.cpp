#include "testrgb.h"
#include <QPainter>
#include <QImage>

static int w = 1280;
static int h = 720;

TestRGB::TestRGB(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    resize(w, h);//改变窗口大小
}

TestRGB::~TestRGB()
{}

void TestRGB::paintEvent(QPaintEvent * ev)
{
    QImage img(w,h,QImage::Format::Format_RGB888);
    auto d = img.bits();
    int r = 255;
    for (int j = 0; j < h; j++)
    {
        int b = j * w * 3;
        for (int i = 0; i < w * 3; i+=3)
        {
            d[b + i] = r;//R
            d[b + i+1] = 0;//G
            d[b + i+2] = 0;//B

            r--;
        }
    }
    QPainter p;
    p.begin(this);

    p.drawImage(0, 0, img);
    p.end();

}

