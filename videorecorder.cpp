#include "videorecorder.h"
#include <QMediaDevices>

VideoRecorder::VideoRecorder(QObject *parent)
    : QObject(parent)
    , camera_device(nullptr)
    , captureSession(new QMediaCaptureSession(this))
    , videoCaptureManager(new VideoCapture(this))
{
    videoCaptureManager->setActiveSession(captureSession);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &VideoRecorder::getDeviceList);
    timer->start(500);
}

VideoRecorder::~VideoRecorder() {
    delete camera_device;
}

void VideoRecorder::setCamera(QCamera* camera)
{
    if (this->camera_device != camera) {
        if (this->camera_device) {
            this->camera_device->stop();
        }
        this->camera_device = camera;
        if (camera) {
            captureSession->setCamera(camera);
            imageCaptureManager->setCamera(camera);
            camera->start();
            qDebug() << "Camera started";
        }
        emit cameraChanged();
    }
}

QCamera* VideoRecorder::getCamera() {
    qDebug() << "GET";
    return camera_device;
}

void VideoRecorder::startStopVideoRecording()
{
    if (!camera_device) {
        qDebug() << "No camera available";
        return;
    }

    if (videoCaptureManager->isRecording()) {
        videoCaptureManager->stopCapturingVideo();
        emit videoRecordingStatusChanged(false);
    } else {
        videoCaptureManager->startCapturingVideo(camera_device);
        emit videoRecordingStatusChanged(true);
    }
}

QMediaCaptureSession* VideoRecorder::getCaptureSession()
{
    return captureSession;
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
        emit frameCaptured("XD");
    }
}


void VideoRecorder::setPath(const QString& path) {
    if (imageCaptureManager) {
        imageCaptureManager->setCaptureImgPath(path);
    } else {
        qDebug() << "ImageCaptureManager isn't initialized";
    }
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

