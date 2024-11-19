#include "videorecorder.h"
#include <QMediaDevices>
#include <QDebug>
#include <QUrl>

VideoRecorder::VideoRecorder(QObject *parent)
    : QObject(parent)
    , camera_device(nullptr)
    , captureSession(new QMediaCaptureSession(this))
    , videoCaptureManager(new VideoCapture(this))
    , camera_list_size(0)
    , isRecording(false)
{
    // Create ImageCapture manager
    imageCaptureManager = new ImageCapture(this);

    // Configure video capture manager
    connect(videoCaptureManager, &VideoCapture::videoCaptured,
            this, [this](const QString& path) {
                qDebug() << "Video saved to:" << path;
                emit videoCaptured(path);
            });

    connect(videoCaptureManager, &VideoCapture::recordingError,
            this, [this](const QString& error) {
                qDebug() << "Recording error:" << error;
                updateRecordingStatus(false);
            });

    // Start device polling timer
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &VideoRecorder::getDeviceList);
    timer->start(500);

    qDebug() << "VideoRecorder initialized";
}

VideoRecorder::~VideoRecorder() {
    if (camera_device) {
        camera_device->stop();
        delete camera_device;
    }
}

void VideoRecorder::updateRecordingStatus(bool recording) {
    if (isRecording != recording) {
        isRecording = recording;
        emit videoRecordingStatusChanged(recording);
        qDebug() << "Recording status changed to:" << recording;
    }
}

void VideoRecorder::setQmlCaptureSession(QObject* session) {
    QMediaCaptureSession* mediaSession = qobject_cast<QMediaCaptureSession*>(session);
    if (mediaSession) {
        qDebug() << "Setting QML capture session";
        if (mediaSession != captureSession) {
            captureSession = mediaSession;
            if (videoCaptureManager) {
                videoCaptureManager->setActiveSession(captureSession);
                qDebug() << "Video capture manager session updated";
            }
            // if (imageCaptureManager) {
            //     imageCaptureManager->setActiveSession(captureSession);
            //     qDebug() << "Image capture manager session updated";
            // }
            emit captureSessionChanged();
        }
    } else {
        qDebug() << "Invalid media session provided";
    }
}

QMediaCaptureSession* VideoRecorder::getCaptureSession() {
    return captureSession;
}

QList<QCameraDevice> VideoRecorder::getDeviceList() {
    QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    if (cameras.size() != camera_list_size) {
        camera_list_size = cameras.size();
        tab_id_list.clear();
        tab_camera_names_list.clear();

        for (const QCameraDevice &cameraDevice : cameras) {
            tab_id_list.append(QVariant(cameraDevice.id()));
            tab_camera_names_list.append(cameraDevice.description());
        }

        emit deviceListChanged(tab_id_list, tab_camera_names_list);
        qDebug() << "Device list updated. Found" << cameras.size() << "cameras";
    }
    return cameras;
}

void VideoRecorder::createCamera(const QString& deviceId) {
    qDebug() << "Creating camera for device ID:" << deviceId;

    if (deviceId.isEmpty()) {
        qDebug() << "Empty device ID provided";
        return;
    }

    current_id = deviceId;
    QCameraDevice device;

    for (const QCameraDevice &cameraDevice : QMediaDevices::videoInputs()) {
        if (cameraDevice.id() == deviceId) {
            device = cameraDevice;
            break;
        }
    }

    if (!device.isNull()) {
        QCamera* newCamera = new QCamera(device);

        newCamera->setCameraFormat(newCamera->cameraDevice().videoFormats().first());

        newCamera->setFocusMode(QCamera::FocusModeAuto);
        newCamera->setExposureMode(QCamera::ExposureAuto);

        setCamera(newCamera);
        qDebug() << "Camera created successfully";
    } else {
        qDebug() << "Failed to find camera device with ID:" << deviceId;
    }
}

void VideoRecorder::setCamera(QCamera* camera) {
    if (this->camera_device != camera) {
        qDebug() << "Setting new camera";

        // Clean up old camera
        if (this->camera_device) {
            this->camera_device->stop();
            delete this->camera_device;
        }

        this->camera_device = camera;

        if (camera) {
            captureSession->setCamera(camera);
            videoCaptureManager->setCamera(camera);
            imageCaptureManager->setCamera(camera);

            if (!camera->cameraDevice().videoFormats().isEmpty()) {
                QCameraFormat format = camera->cameraDevice().videoFormats().first();
                camera->setCameraFormat(format);
            }

            QTimer::singleShot(100, this, [this, camera]() {
                camera->start();
                qDebug() << "Camera started with status:" << camera->isActive();

                if (!camera->isActive()) {
                    QTimer::singleShot(500, this, [this, camera]() {
                        if (camera && !camera->isActive()) {
                            camera->stop();
                            camera->start();
                            qDebug() << "Camera restart attempt completed";
                        }
                    });
                }
            });

            qDebug() << "Camera setup completed in VideoRecorder";
        } else {
            qDebug() << "Null camera provided";
        }

        emit cameraChanged();
    }
}


void VideoRecorder::captureFrame() {
    if (!camera_device) {
        qDebug() << "No camera available for frame capture";
        return;
    }

    QString currentCameraId = camera_device->cameraDevice().id();
    bool wasRecording = isRecording;

    if (wasRecording) {
        qDebug() << "Pausing recording for frame capture";
        videoCaptureManager->stopCapturingVideo();
        updateRecordingStatus(false);
    }

    qDebug() << "Initiating frame capture";
    emit frameCapture(camera_device);

    QTimer::singleShot(500, this, [this, currentCameraId, wasRecording]() {
        createCamera(currentCameraId);

        if (wasRecording) {
            QTimer::singleShot(500, this, [this]() {
                qDebug() << "Resuming recording after frame capture";
                if (videoCaptureManager->startCapturingVideo(camera_device)) {
                    updateRecordingStatus(true);
                }
            });
        }
    });
}

void VideoRecorder::startStopVideoRecording() {
    if (!camera_device) {
        qDebug() << "No camera available for recording";
        return;
    }

    if (isRecording) {
        qDebug() << "Stopping recording...";
        videoCaptureManager->stopCapturingVideo();
        updateRecordingStatus(false);
    } else {
        qDebug() << "Starting recording...";
        if (videoCaptureManager->startCapturingVideo(camera_device)) {
            updateRecordingStatus(true);
        } else {
            qDebug() << "Failed to start recording";
            updateRecordingStatus(false);
        }
    }
}

QCamera* VideoRecorder::getCamera() {
    return camera_device;
}

void VideoRecorder::setPath(const QString& path) {
    if (!path.isEmpty()) {
        qDebug() << "Setting new media path:" << path;
        emit pathChanged(path);
    } else {
        qDebug() << "Empty path provided";
    }
}
