#include "devicecheck.h"

DeviceCheck::DeviceCheck() {


    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DeviceCheck::getDeviceList);

    timer->start(500);

}

QList<QCameraDevice> DeviceCheck::getDeviceList(){
    QList<QCameraDevice> availableCameras = QMediaDevices::videoInputs();
    tab_id_list.clear();

    if (availableCameras.isEmpty()) {
        qDebug() << "No cameras available";
    } else {
        for (const QCameraDevice &cameraDevice : availableCameras) {
            qDebug() << "Camera Name:" << cameraDevice.description();
        }
    }

    for (const QCameraDevice &cameraDevice : availableCameras) {
        DeviceCheck::tab_id_list.append(cameraDevice.id());
    }

    emit deviceListChanged(tab_id_list);

    return availableCameras;
}


