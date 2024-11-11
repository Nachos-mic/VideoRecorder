#ifndef IMAGECAPTURE_H
#define IMAGECAPTURE_H

#include <QObject>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QCamera>
#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <QtCore>
#include <QFileDialog>
#include <QUrl>


class ImageCapture : public QObject {
    Q_OBJECT

public:
    explicit ImageCapture(QObject *parent = nullptr);
    ~ImageCapture();
    void setCamera(QCamera* camera);

public slots:
    void captureFrame(QCamera* camera);
    void setCaptureImgPath(QString path);

private:
    QString generateFileName() const;
    QMediaCaptureSession captureSession;
    QImageCapture* imageCapture;
    QString img_path;

signals:
    void frameCaptured(const QString& path);
    void setCaptureImgPathChanged(QString path);
};

#endif // IMAGECAPTURE_H
