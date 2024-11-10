#include "videorecorder.h"
#include <QMediaDevices>

VideoRecorder::VideoRecorder(QObject *parent)
    : QObject(parent)
    , camera_device(nullptr)
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &VideoRecorder::getDeviceList);
    timer->start(500);
}

VideoRecorder::~VideoRecorder(){
    delete camera_device;
}

QList<QCameraDevice> VideoRecorder::getDeviceList() {
    QList<QCameraDevice> availableCameras = QMediaDevices::videoInputs();
    tab_id_list.clear();
    tab_camera_names_list.clear();

    if (availableCameras.isEmpty()) {
        qDebug() << "No cameras available";
    }

    for (const QCameraDevice &cameraDevice : availableCameras) {
        QString cameraId = QString::fromUtf8(cameraDevice.id());
        tab_id_list.append(cameraId);
        tab_camera_names_list.append(cameraDevice.description());
    }

    if(camera_list_size != availableCameras.size()){
        camera_list_size = availableCameras.size();
        emit deviceListChanged(tab_id_list, tab_camera_names_list);
    }

    return availableCameras;
}

QCamera* VideoRecorder::getCamera() {
    qDebug() << "GET";
    return camera_device;
}

void VideoRecorder::setCamera(QCamera* camera) {
    qDebug() << "SET";
    if (camera_device != camera) {
        delete camera_device;
        camera_device = camera;
        if (camera_device) {
            imageCaptureManager->setCamera(camera_device);
            camera_device->start();
            qDebug() << "Camera started";
        }
        emit cameraChanged();
    }
}

void VideoRecorder::createCamera(const QString& deviceId) {
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

void VideoRecorder::captureFrame() {
    if (!camera_device) {
        qDebug() << "No camera available";
        return;
    }

    if (!camera_device->isActive()) {
        qDebug() << "Camera is not active, attempting to start...";
        camera_device->start();
        QTimer::singleShot(1500, this, [this]() {
            if (camera_device && camera_device->isActive()) {
                qDebug() << "Capturing Frame";
                imageCaptureManager->captureFrame(camera_device);
            } else {
                qDebug() << "Camera failed to activate";
            }
        });
    } else {
        qDebug() << "Capturing Frame";
        imageCaptureManager->captureFrame(camera_device);
    }
}

void VideoRecorder::setPath(QString path) {
    if (imageCaptureManager) {
        imageCaptureManager->setCaptureImgPath(path);
        qDebug() << "Set Path:" << path;
    } else {
        qDebug() << "ImageCaptureManager isn't initialized";
    }
}

