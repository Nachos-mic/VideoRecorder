#include "imagecapture.h"
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>

ImageCapture::ImageCapture(QObject *parent)
    : QObject(parent)
    , imageCapture(new QImageCapture(this))
{
    connect(imageCapture, &QImageCapture::imageSaved, this,
            [this](int id, const QString& path) {
                qDebug() << "Image saved at:" << path;
                emit frameCaptured(path);
            }
            );
}

ImageCapture::~ImageCapture() {

}

void ImageCapture::setCamera(QCamera* camera) {
    if (camera) {
        captureSession.setCamera(camera);
        captureSession.setImageCapture(imageCapture);
    }
}

QString ImageCapture::generateFileName() const {
    QString path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    QDir().mkpath(path);

    return path + "/screenshoot_" +
           QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") +
           ".jpg";
}

void ImageCapture::captureFrame() {
    if (!imageCapture) {
        qDebug() << "Image capture not initialized";
        return;
    }

    QString filename = generateFileName();
    imageCapture->captureToFile(filename);
}
