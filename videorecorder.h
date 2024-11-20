#ifndef VIDEORECORDER_H
#define VIDEORECORDER_H

#include <QObject>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QMediaRecorder>
#include <QMediaDevices>
#include <QTimer>
#include <QThread>
#include "videocapture.h"
#include "imagecapture.h"

class VideoRecorder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QCamera* previewCamera READ getPreviewCamera NOTIFY previewCameraChanged)
    Q_PROPERTY(QCamera* camera_device READ getCamera NOTIFY cameraChanged)

public:
    explicit VideoRecorder(QObject *parent = nullptr);
    ~VideoRecorder();

    Q_INVOKABLE void createCamera(const QString& deviceId);
    Q_INVOKABLE void startStopVideoRecording();
    Q_INVOKABLE void captureFrame();
    Q_INVOKABLE void setPath(const QString& path);

    QCamera* getCamera();
    QCamera* getPreviewCamera();
    QCamera* camera_device;

    ImageCapture* imageCaptureManager;

public slots:
    void handleRecordingStateChanged(bool isRecording);

signals:
    void cameraChanged();
    void deviceListChanged(QVariantList id_list, QVariantList name_list);
    void frameCapture(QCamera* camera);
    void pathChanged(const QString& path);
    void videoCaptured(const QString& path);
    void videoRecordingStatusChanged(bool isRecording);
    void previewCameraChanged();

private:
    void setCamera(QCamera* camera);
    QList<QCameraDevice> getDeviceList();
    void setupPreviewSession();
    void cleanupPreviewSession();

    QMediaCaptureSession* captureSession;
    QMediaCaptureSession* previewSession;
    VideoCapture* videoCaptureManager;
    QString current_id;
    int camera_list_size;
    QVariantList tab_id_list;
    QVariantList tab_camera_names_list;
    QCamera* previewCamera;
};

#endif // VIDEORECORDER_H
