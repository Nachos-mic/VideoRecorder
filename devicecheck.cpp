#include "devicecheck.h"

DeviceCheck::DeviceCheck() {


    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DeviceCheck::getDeviceList);

    timer->start(5000);

}

QList<QCameraDevice> DeviceCheck::getDeviceList(){
    QList<QCameraDevice> availableCameras = QMediaDevices::videoInputs();

    if (availableCameras.isEmpty()) {
        qDebug() << "No cameras available";
    } else {
        for (const QCameraDevice &cameraDevice : availableCameras) {
            qDebug() << "Camera Name:" << cameraDevice.description();
        }
    }
    return availableCameras;
}
