#include "imagecapture.h"
#include "qthread.h"

ImageCapture::ImageCapture(QObject *parent)
    : QObject(parent)
    , imageCapture(new QImageCapture(this))
    , img_path(QDir::currentPath() + "/captures")
{
    connect(imageCapture, &QImageCapture::imageSaved,
            this, [this](int id, const QString& path) {
                qDebug() << "Image saved successfully at:" << path;
                emit frameCaptured(path);
            });

    connect(imageCapture, &QImageCapture::errorOccurred,
            this, [](int id, QImageCapture::Error error, const QString &errorString) {
                qDebug() << "Image capture error:" << errorString;
            });
}

ImageCapture::~ImageCapture() {
}

void ImageCapture::setCamera(QCamera* camera) {
    if (camera) {
        captureSession.setCamera(camera);
        captureSession.setImageCapture(imageCapture);
        qDebug() << "Camera and image capture set in capture session";
    }
}

QString ImageCapture::generateFileName() const {
    QString path = img_path;
    QDir().mkpath(path);
    return path + "/screenshot_" +
           QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") +
           ".jpg";
}

void ImageCapture::captureFrame(QCamera* camera) {

    captureSession.setCamera(camera);
    captureSession.setImageCapture(imageCapture);

    if (!imageCapture) {
        qDebug() << "Image capture not initialized";
        return;
    }

    if (!imageCapture->isReadyForCapture()) {
        qDebug() << "Image capture not ready";
        return;
    }

    QString filename = generateFileName();
    qDebug() << "Attempting to capture to:" << filename;
    imageCapture->captureToFile(filename);

    camera->stop();
    QThread::msleep(100);
    camera->start();
}

void ImageCapture::setCaptureImgPath(QString path) {
    if (img_path != path) {
        img_path = path;
        QDir().mkpath(path);
        emit setCaptureImgPathChanged(path);
        qDebug() << "Capture path set to:" << path;
    }
}
