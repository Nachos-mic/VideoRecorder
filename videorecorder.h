#ifndef VIDEORECORDER_H
#define VIDEORECORDER_H

#include <QObject>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QTimer>
#include "imagecapture.h"

class VideoRecorder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QCamera* camera_device READ getCamera NOTIFY cameraChanged)
    Q_PROPERTY(QCamera* previewCamera READ getPreviewCamera NOTIFY previewCameraChanged)

public:
    explicit VideoRecorder(QObject *parent = nullptr);
    ~VideoRecorder();

    ImageCapture* imageCaptureManager;
    QMediaCaptureSession* captureSession;
    QCamera* camera_device;
    QCamera* previewCamera;

    Q_INVOKABLE void createCamera(const QString& deviceId);
    Q_INVOKABLE void captureFrame();
    Q_INVOKABLE void setPath(const QString& path);
    Q_INVOKABLE QString getPath();
    Q_INVOKABLE QCamera* getPreviewCamera();
    Q_INVOKABLE QCamera* getCamera();

signals:
    void deviceListChanged(QStringList id_list, QStringList name_list);
    void cameraChanged();
    void previewCameraChanged();
    void frameCapture(QCamera* camera);
    void pathChanged(const QString& path);

private:
    QMediaCaptureSession* previewSession;
    QString current_id;
    int camera_list_size = 0;
    QStringList tab_id_list;
    QStringList tab_camera_names_list;

    void setCamera(QCamera* camera);
    QList<QCameraDevice> getDeviceList();
    void setupPreviewSession();
    void cleanupPreviewSession();
};

#endif // VIDEORECORDER_H
