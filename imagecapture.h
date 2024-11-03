#ifndef IMAGECAPTURE_H
#define IMAGECAPTURE_H

#include <QObject>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QCamera>

class ImageCapture : public QObject {
    Q_OBJECT

public:
    explicit ImageCapture(QObject *parent = nullptr);
    ~ImageCapture();

    void setCamera(QCamera* camera);
    void captureFrame();
    QImageCapture* getImageCapture() const { return imageCapture; }
    QMediaCaptureSession* getCaptureSession() { return &captureSession; }

private:
    QImageCapture* imageCapture;
    QMediaCaptureSession captureSession;
    QString generateFileName() const;

signals:
    void frameCaptured(const QString& path);
};

#endif // IMAGECAPTURE_H
