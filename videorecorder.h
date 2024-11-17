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
    Q_PROPERTY(QMediaCaptureSession* captureSession READ getCaptureSession CONSTANT)

public:
    explicit VideoRecorder(QObject *parent = nullptr);
    ~VideoRecorder();
    ImageCapture* imageCaptureManager;
    QList<QCameraDevice> getDeviceList();
    QCamera* getCamera();
    QMediaCaptureSession* getCaptureSession();
    QList<QString> tab_id_list;
    QStringList tab_camera_names_list;
    Q_INVOKABLE void setCamera(QCamera* camera);
    Q_INVOKABLE void createCamera(const QString& deviceId);
    Q_INVOKABLE void startStopVideoRecording();

private:
    QCamera* camera_device;
    QMediaCaptureSession* captureSession;
    int camera_list_size = 0;
    VideoCapture* videoCaptureManager;

public slots:
    void captureFrame();
    void setPath(const QString& path);

signals:
    void deviceListChanged(QList<QString> ids, QStringList names);
    void cameraChanged();
    void frameCapture(QCamera* camera);
    void pathChanged(QString path);
    void frameCaptured(QString path);
    void videoCaptured(QString path);
    void videoRecordingStatusChanged(bool isRecording);
};

#endif // VIDEORECORDER_H
