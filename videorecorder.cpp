#include "videorecorder.h"
#include <QMediaDevices>

VideoRecorder::VideoRecorder(QObject *parent)
    : QObject(parent)
    , camera_device(nullptr)
    , captureSession(new QMediaCaptureSession(this))
    , previewSession(new QMediaCaptureSession(this))
    , previewCamera(nullptr)
    , imageCaptureManager(nullptr)
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &VideoRecorder::getDeviceList);
    timer->start(500);
}

VideoRecorder::~VideoRecorder()
{
    cleanupPreviewSession();
    if (camera_device) {
        camera_device->stop();
        delete camera_device;
    }
}

void VideoRecorder::setupPreviewSession()
{
    if (!current_id.isEmpty()) {
        QCameraDevice device;
        for (const QCameraDevice &cameraDevice : QMediaDevices::videoInputs()) {
            if (cameraDevice.id() == current_id) {
                device = cameraDevice;
                break;
            }
        }

        if (!device.isNull()) {
            if (previewCamera) {
                cleanupPreviewSession();
            }
            previewCamera = new QCamera(device);
            previewSession->setCamera(previewCamera);
            previewCamera->start();
            emit previewCameraChanged();
        }
    }
}

void VideoRecorder::cleanupPreviewSession()
{
    if (previewCamera) {
        previewCamera->stop();
        previewSession->setCamera(nullptr);
        delete previewCamera;
        previewCamera = nullptr;
        emit previewCameraChanged();
    }
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
            captureSession->setCamera(camera);
            if (imageCaptureManager) {
                imageCaptureManager->setCamera(camera);
            }
            camera->start();
            qDebug() << "Camera started in VideoRecorder";
        }
        emit cameraChanged();
    }
}

void VideoRecorder::captureFrame() {
    if (!camera_device || !imageCaptureManager) {
        qDebug() << "No camera or image capture manager available";
        return;
    }

    QString currentCameraId = camera_device->cameraDevice().id();
    qDebug() << "Capturing Frame";
    emit frameCapture(camera_device);

    QTimer::singleShot(500, this, [this, currentCameraId]() {
        if (camera_device) {
            camera_device->stop();
            delete camera_device;
            camera_device = nullptr;
        }

        createCamera(currentCameraId);
    });
}

QCamera* VideoRecorder::getPreviewCamera()
{
    return previewCamera;
}

QCamera* VideoRecorder::getCamera() {
    return camera_device;
}

void VideoRecorder::setPath(const QString& path) {
    emit pathChanged(path);
}

QString VideoRecorder::getPath(){
    qDebug() << "GETPATH";
    return Utils::getMediaPath();
};
