#include "imagecapture.h"

ImageCapture::ImageCapture(QObject *parent)
    : QObject(parent)
    , imageCapture(new QImageCapture(this))
{
    connect(imageCapture, &QImageCapture::imageSaved,
            this, [this](int id, const QString& path) {
                qDebug() << "Image saved at:" << path;
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
        currentCamera = camera;

        captureSession.setCamera(camera);
        captureSession.setImageCapture(imageCapture);
        qDebug() << "ImageCapture: CaptureSession configured";
    }
}

void ImageCapture::captureFrame(QCamera* camera) {
    if (!camera || !imageCapture) {
        qDebug() << "Camera or ImageCapture not available";
        return;
    }

    currentCamera = camera;
    captureSession.setCamera(camera);
    captureSession.setImageCapture(imageCapture);

    QTimer::singleShot(100, this, [this]() {
        if (!imageCapture->isReadyForCapture()) {
            qDebug() << "ImageCapture is not ready for capture";
            return;
        }

        QString filename = generateFileName();
        qDebug() << "Capturing frame to file:" << filename;
        imageCapture->captureToFile(filename);
    });
}

QString ImageCapture::generateFileName() const {
    QString path = Utils::getMediaPath();
    QDir().mkpath(path);
    return path + "/screenshot_" +
           QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") +
           ".jpg";
}

void ImageCapture::setCaptureImgPath(QString path) {
    path = QUrl(path).toLocalFile();

    if (Utils::getMediaPath() != path) {
        Utils::setMediaPath(path);
        QDir().mkpath(path);
        emit setCaptureImgPathChanged(path);
        qDebug() << "Path Set:" << path;
    }
}
