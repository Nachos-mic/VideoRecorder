#ifndef VIDEORECORDER_H
#define VIDEORECORDER_H

#include <QObject>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QTimer>
#include "videocapture.h"
#include "imagecapture.h"

class VideoRecorder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QCamera* camera READ getCamera NOTIFY cameraChanged)
    Q_PROPERTY(QMediaCaptureSession* captureSession READ getCaptureSession NOTIFY captureSessionChanged)

public:
    explicit VideoRecorder(QObject *parent = nullptr);
    ~VideoRecorder();

    Q_INVOKABLE void createCamera(const QString& deviceId);
    Q_INVOKABLE void startStopVideoRecording();
    Q_INVOKABLE void captureFrame();
    Q_INVOKABLE void setPath(const QString& path);
    Q_INVOKABLE QList<QCameraDevice> getDeviceList();
    Q_INVOKABLE void setQmlCaptureSession(QObject* session);

    QCamera* getCamera();
    QMediaCaptureSession* getCaptureSession();

    ImageCapture* imageCaptureManager;
    VideoCapture* videoCaptureManager;

signals:
    void cameraChanged();
    void captureSessionChanged();
    void deviceListChanged(const QVariantList& deviceList, const QStringList& namesList);
    void videoCaptured(const QString& path);
    void videoRecordingStatusChanged(bool isRecording);
    void frameCapture(QCamera* camera);
    void pathChanged(const QString& path);

private:
    void setCamera(QCamera* camera);
    void updateRecordingStatus(bool recording);

    QCamera* camera_device;
    QMediaCaptureSession* captureSession;
    QString current_id;
    int camera_list_size;
    QVariantList tab_id_list;
    QStringList tab_camera_names_list;
    bool isRecording;
};

#endif // VIDEORECORDER_H
