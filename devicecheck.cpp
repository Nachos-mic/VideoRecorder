#include "devicecheck.h"
#include <QMediaDevices>

DeviceCheck::DeviceCheck()
    : camera_device(nullptr)
    , imageCaptureManager(new ImageCapture(this))
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DeviceCheck::getDeviceList);
    timer->start(500);

    // Connect the ImageCapture signals to our signals
    connect(imageCaptureManager, &ImageCapture::frameCaptured,
            this, &DeviceCheck::frameCaptured);
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

    emit deviceListChanged(tab_id_list, tab_camera_names_list);
    return availableCameras;
}

QCamera* DeviceCheck::getCamera() {
    qDebug() << "GET";
    return camera_device;
}

void DeviceCheck::createCamera(const QString& deviceId) {
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    for (const QCameraDevice& device : cameras) {
        if (device.id() == deviceId) {
            QCamera* newCamera = new QCamera(device);
            setCamera(newCamera);
            if (camera_device) {
                camera_device->start();
                qDebug() << "Camera created and started:" << device.description();
            }
            return;
        }
    }
    qDebug() << "Failed to find camera with ID:" << deviceId;
}

void DeviceCheck::setCamera(QCamera* camera) {
    qDebug() << "SET";
    if (camera_device != camera) {
        delete camera_device;
        camera_device = camera;

        if (camera_device) {
            imageCaptureManager->setCamera(camera_device);
            qDebug() << "Camera set in ImageCapture";
        }

        emit cameraChanged();
    }
}

void DeviceCheck::captureFrame() {
    if (camera_device && camera_device->isActive()) {
        qDebug() << "Capturing frame from active camera";
        imageCaptureManager->captureFrame();
    } else {
        qDebug() << "No active camera available for capture";
        if (camera_device && !camera_device->isActive()) {
            camera_device->start();
            QTimer::singleShot(500, this, [this]() {
                imageCaptureManager->captureFrame();
            });
        }
    }
}
