#include "devicecheck.h"
#include <QMediaDevices>

DeviceCheck::DeviceCheck()
    : camera_device(nullptr)
    , imageCaptureManager(new ImageCapture(this))
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DeviceCheck::getDeviceList);
    timer->start(500);

    connect(imageCaptureManager, &ImageCapture::frameCaptured,
            this, &DeviceCheck::frameCaptured);

    connect(imageCaptureManager, &ImageCapture::setCaptureImgPathChanged,
            this, &DeviceCheck::setPath);
}

DeviceCheck::~DeviceCheck() {
    delete camera_device;
}

QList<QCameraDevice> DeviceCheck::getDeviceList() {
    QList<QCameraDevice> availableCameras = QMediaDevices::videoInputs();
    tab_id_list.clear();
    tab_camera_names_list.clear();

    if (availableCameras.isEmpty()) {
        qDebug() << "No cameras available";
    }

    for (const QCameraDevice &cameraDevice : availableCameras) {
        tab_id_list.append(cameraDevice.id());
        tab_camera_names_list.append(cameraDevice.description());
    }

    if(camera_list_size != availableCameras.size()){
        camera_list_size = availableCameras.size();
        emit deviceListChanged(tab_id_list, tab_camera_names_list);
    }
    return availableCameras;
}

QCamera* DeviceCheck::getCamera() {
    qDebug() << "GET";
    return camera_device;
}

void DeviceCheck::setCamera(QCamera* camera) {
    qDebug() << "SET";
    if (camera_device != camera) {
        delete camera_device;
        camera_device = camera;
        if (camera_device) {
            imageCaptureManager->setCamera(camera_device);
            camera_device->start();
            qDebug() << "Camera set and started";
        }
        emit cameraChanged();
    }
}

void DeviceCheck::createCamera(const QString& deviceId) {
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    for (const QCameraDevice& device : cameras) {
        if (device.id() == deviceId) {
            QCamera* newCamera = new QCamera(device);
            setCamera(newCamera);
            return;
        }
    }
    qDebug() << "Failed to find camera with ID:" << deviceId;
}

void DeviceCheck::captureFrame() {
    if (!camera_device) {
        qDebug() << "No camera device available";
        return;
    }

    if (!camera_device->isActive()) {
        qDebug() << "Camera is not active, attempting to start...";
        camera_device->start();
        QTimer::singleShot(1500, this, [this]() {
            if (camera_device && camera_device->isActive()) {
                qDebug() << "Camera is now active, capturing frame";
                imageCaptureManager->captureFrame(camera_device);
            } else {
                qDebug() << "Camera failed to become active";
            }
        });
    } else {
        qDebug() << "Camera is active, capturing frame";
        imageCaptureManager->captureFrame(camera_device);
    }
}

void DeviceCheck::setPath(QString path) {
    if (imageCaptureManager) {
        imageCaptureManager->setCaptureImgPath(path);
        qDebug() << "Path set in DeviceCheck:" << path;
    } else {
        qDebug() << "ImageCaptureManager is not initialized";
    }
}

