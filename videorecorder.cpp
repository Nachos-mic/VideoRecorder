#include "videorecorder.h"
#include <QMediaDevices>

VideoRecorder::VideoRecorder(QObject *parent)
    : QObject(parent)
    , camera_device(nullptr)
    , captureSession(new QMediaCaptureSession(this))
    , videoCaptureManager(new VideoCapture(this))
{
    videoCaptureManager->setActiveSession(captureSession);

    connect(videoCaptureManager, &VideoCapture::videoCaptured,
            this, [this](const QString& path) {
                emit videoCaptured(path);
            });

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &VideoRecorder::getDeviceList);
    timer->start(500);
}

VideoRecorder::~VideoRecorder() {
    delete camera_device;
}

QList<QCameraDevice> VideoRecorder::getDeviceList() {
    QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    if (cameras.size() != camera_list_size) {
        camera_list_size = cameras.size();
        tab_id_list.clear();
        tab_camera_names_list.clear();

        for (const QCameraDevice &cameraDevice : cameras) {
            tab_id_list.append(cameraDevice.id());
            tab_camera_names_list.append(cameraDevice.description());
        }

        emit deviceListChanged(tab_id_list, tab_camera_names_list);
    }
    return cameras;
}

void VideoRecorder::createCamera(const QString& deviceId) {

    current_id = deviceId;
    QCameraDevice device;
    for (const QCameraDevice &cameraDevice : QMediaDevices::videoInputs()) {
        if (cameraDevice.id() == deviceId) {
            device = cameraDevice;
            break;
        }
    }

    if (!device.isNull()) {
        setCamera(new QCamera(device));
    }
}

void VideoRecorder::setCamera(QCamera* camera)
{
    if (this->camera_device != camera) {
        if (this->camera_device) {
            this->camera_device->stop();
            delete this->camera_device;
        }

        this->camera_device = camera;

        if (camera) {
            // First set up the camera in the main capture session
            captureSession->setCamera(camera);
            videoCaptureManager->setCamera(camera);

            // Then set up image capture separately
            imageCaptureManager->setCamera(camera);

            // Start the camera
            camera->start();
            qDebug() << "Camera started in VideoRecorder";
        }
        emit cameraChanged();
    }
}

void VideoRecorder::captureFrame() {
    if (!camera_device) {
        qDebug() << "No camera available";
        return;
    }

    // Store the current camera ID before stopping it
    QString currentCameraId = camera_device->cameraDevice().id();
    bool wasRecording = videoCaptureManager->isRecording();

    // If we're recording, stop recording temporarily
    if (wasRecording) {
        videoCaptureManager->stopCapturingVideo();
        emit videoRecordingStatusChanged(false);
    }

    // Capture the frame
    qDebug() << "Capturing Frame";
    imageCaptureManager->captureFrame(camera_device);

    // Wait for capture to complete, then recreate camera
    QTimer::singleShot(500, this, [this, currentCameraId, wasRecording]() {
        // Stop and delete the old camera
        if (camera_device) {
            camera_device->stop();
            delete camera_device;
            camera_device = nullptr;
        }

        // Create and set up the new camera
        QCameraDevice device;
        for (const QCameraDevice &cameraDevice : QMediaDevices::videoInputs()) {
            if (cameraDevice.id() == currentCameraId) {
                device = cameraDevice;
                break;
            }
        }

        if (!device.isNull()) {
            QCamera* newCamera = new QCamera(device);
            setCamera(newCamera);  // This will handle all the necessary setup

            // If we were recording, resume after camera is recreated
            if (wasRecording) {
                QTimer::singleShot(500, this, [this]() {
                    videoCaptureManager->startCapturingVideo(camera_device);
                    emit videoRecordingStatusChanged(true);
                });
            }
        } else {
            qDebug() << "Failed to recreate camera with ID:" << currentCameraId;
        }
    });
}

void VideoRecorder::startStopVideoRecording()
{
    if (!camera_device) {
        qDebug() << "No camera available";
        return;
    }

    if (videoCaptureManager->isRecording()) {
        qDebug() << "Stopping recording...";
        videoCaptureManager->stopCapturingVideo();
        emit videoRecordingStatusChanged(false);
    } else {
        qDebug() << "Starting recording...";
        if (!camera_device->isActive()) {
            camera_device->start();
            QTimer::singleShot(500, this, [this]() {
                videoCaptureManager->startCapturingVideo(camera_device);
                emit videoRecordingStatusChanged(true);
            });
        } else {
            videoCaptureManager->startCapturingVideo(camera_device);
            emit videoRecordingStatusChanged(true);
        }
    }
}

QCamera* VideoRecorder::getCamera() {
    qDebug() << "GET";
    return camera_device;
}

QMediaCaptureSession* VideoRecorder::getCaptureSession() {
    return captureSession;
}

void VideoRecorder::setPath(const QString& path) {
    emit pathChanged(path);
}
