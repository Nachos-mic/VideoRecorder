#ifndef VIDEORECORDER_H
#define VIDEORECORDER_H

#include <QtMultimedia>
#include <QCamera>
#include <QCameraDevice>
#include <QDebug>
#include <QTimer>
#include <QMediaRecorder>
#include <QtCore>
#include "imagecapture.h"
#include "videocapture.h"

class VideoRecorder : public QObject {
    Q_OBJECT
    Q_PROPERTY(QCamera* camera_device READ getCamera WRITE setCamera NOTIFY cameraChanged)

public:
    explicit VideoRecorder(QObject *parent = nullptr);
    ~VideoRecorder();
    ImageCapture* imageCaptureManager;
    VideoCapture* videoCaptureManager;
    QList<QCameraDevice> getDeviceList();
    QCamera* getCamera();
    void setCamera(QCamera* camera);
    QList<QString> tab_id_list;
    QStringList tab_camera_names_list;

private:
    QCamera* camera_device;
    int camera_list_size = 0;

public slots:
    void createCamera(const QString& deviceId);
    void captureFrame();
    void startCapturingVideo();
    void stopCapturingVideo();
    void setPath(const QString& path);

signals:
    void deviceListChanged(QList<QString> ids, QStringList names);
    void cameraChanged();
    void frameCapture(QCamera* camera);
    void pathChanged(QString path);
    void frameCaptured(QString path);
    void videoCaptured(QString path);
};
#endif // VIDEORECORDER_H
