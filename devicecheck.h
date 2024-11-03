#ifndef DEVICECHECK_H
#define DEVICECHECK_H

#include <QtMultimedia>
#include <QCamera>
#include <QCameraDevice>
#include <QDebug>
#include <QTimer>
#include "imagecapture.h"

class DeviceCheck : public QObject {
    Q_OBJECT
    Q_PROPERTY(QCamera* camera_device READ getCamera WRITE setCamera NOTIFY cameraChanged)

public:
    DeviceCheck();
    ~DeviceCheck();

    QList<QCameraDevice> getDeviceList();
    Q_INVOKABLE QCamera* getCamera();
    void setCamera(QCamera* camera);

    Q_INVOKABLE void createCamera(const QString& deviceId);
    Q_INVOKABLE void captureFrame();

private:
    QList<QString> tab_id_list;
    QList<QString> tab_camera_names_list;
    QCamera* camera_device;
    ImageCapture* imageCaptureManager;

signals:
    void deviceListChanged(QList<QString> tab_camera_id_list, QList<QString> tab_camera_names_list);
    void cameraChanged();
    void frameCaptured(const QString& path);
};

#endif // DEVICECHECK_H
