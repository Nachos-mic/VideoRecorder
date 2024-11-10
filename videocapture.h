#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <QObject>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QCamera>
#include <QDateTime>
#include <QDir>
#include <QDebug>

class VideoCapture : public QObject {
    Q_OBJECT
public:
    explicit VideoCapture(QObject *parent = nullptr);
    ~VideoCapture();

    void setCamera(QCamera* camera);
    void captureVideo(QCamera* camera);
    void setCaptureVideoPath(QString path);

private:
    QString generateFileName() const;
    QMediaCaptureSession captureSession;
    QImageCapture* imageCapture;
    QString vid_path;

signals:
    void frameCaptured(const QString& path);
    void setCaptureImgPathChanged(QString path);
};

#endif // VIDEOCAPTURE_H
