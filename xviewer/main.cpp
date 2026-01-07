#include "xviewer.h"
#include "xcamera_config.h"
#include "xcamera_record.h"

#include <QtWidgets/QApplication>
#include <QDebug>
#include <QDir>

#define TEST_CONFIG_FILE "test.db"

int main(int argc, char *argv[])
{
    const char* save_path = "./video/0/";
    QDir dir;
    dir.mkpath(save_path);

    XCameraRecord camera_record;
    camera_record.set_rtsp_url("rtmp://liteavapp.qcloud.com/live/liteavdemoplayerstreamid");
    //camera_record.set_save_path("record_test.mp4");
    camera_record.set_save_path(save_path);

    camera_record.Start();
    //auto xc = XCameraConfig::Instance();
    //xc->Load(TEST_CONFIG_FILE);

    //XCameraData cd;
    //strcpy_s(cd.name, "camera1");
    //strcpy_s(cd.save_path, ".\\camera1\\");
    //strcpy_s(cd.url, "rtsp://stream.strba.sk:1935/strba/VYHLAD_JAZERO.stream");
    //strcpy_s(cd.sub_url, "rtsp://stream.strba.sk:1935/strba/VYHLAD_JAZERO.stream");

    //XCameraData cd2;
    //strcpy_s(cd2.name, "camera2");
    //strcpy_s(cd2.save_path, ".\\camera2\\");
    //strcpy_s(cd2.url, "rtsp://stream.strba.sk:1935/strba/VYHLAD_JAZERO.stream");
    //strcpy_s(cd2.sub_url, "rtsp://stream.strba.sk:1935/strba/VYHLAD_JAZERO.stream");

    //xc->Push(cd);
    //xc->Push(cd2);

    //int cam_count = xc->GetCamCount();
    //for (int i = 0; i < cam_count; i++)
    //{
    //    auto cam = xc->GetCam(i);
    //    qDebug() << cam.name;
    //}

    //auto c2 = xc->GetCam(1);
    //strcpy_s(c2.name, "camera002");
    //xc->SetCam(1, c2);
    //qDebug() << "change the camera2 name";
    //cam_count = xc->GetCamCount();
    //for (int i = 0; i < cam_count; i++)
    //{
    //    auto cam = xc->GetCam(i);
    //    qDebug() << cam.name;
    //}

    //xc->Save(TEST_CONFIG_FILE);

    //xc->DelCam(1);
    //qDebug() << "delete the camera2 name";
    //cam_count = xc->GetCamCount();
    //for (int i = 0; i < cam_count; i++)
    //{
    //    auto cam = xc->GetCam(i);
    //    qDebug() << cam.name;
    //}

    QApplication app(argc, argv);
    XViewer window;
    window.show();
    return app.exec();
}
