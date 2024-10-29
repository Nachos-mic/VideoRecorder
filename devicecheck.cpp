#include "devicecheck.h"

DeviceCheck::DeviceCheck() {


    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DeviceCheck::getDeviceList);

    timer->start(500);

}

QList<QCameraDevice> DeviceCheck::getDeviceList(){
    QList<QCameraDevice> availableCameras = QMediaDevices::videoInputs();
    tab_id_list.clear();
    tab_camera_names_list.clear();

    if (availableCameras.isEmpty()) {
        qDebug() << "No cameras available";
    }

    for (const QCameraDevice &cameraDevice : availableCameras) {
        DeviceCheck::tab_id_list.append(cameraDevice.id());
        DeviceCheck::tab_camera_names_list.append(cameraDevice.description());
    }

    emit deviceListChanged(tab_id_list,tab_camera_names_list);

    return availableCameras;
}


