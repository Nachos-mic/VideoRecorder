// videorecorder.h
#ifndef VIDEORECORDER_H
#define VIDEORECORDER_H

#include <QObject>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QVideoSink>
#include <QVideoFrame>
#include <QImage>
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>

class VideoRecorder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList cameraList READ getCameraList NOTIFY cameraListChanged)
    Q_PROPERTY(QString currentPath READ getCurrentPath WRITE setCurrentPath NOTIFY currentPathChanged)
    Q_PROPERTY(QString frame READ getFrame NOTIFY frameChanged)

public:
    explicit VideoRecorder(QObject *parent = nullptr);
    ~VideoRecorder();

    QStringList getCameraList() const { return m_cameraList; }
    QString getCurrentPath() const { return m_currentPath; }
    QString getFrame() const;

public slots:
    void setCurrentPath(const QString &path);
    void setCamera(int index);
    void captureFrame();

signals:
    void cameraListChanged();
    void currentPathChanged();
    void frameChanged();

private slots:
    void handleFrameChanged(const QVideoFrame &frame);

private:
    void updateCameraList();
    QCamera *m_camera;
    QMediaCaptureSession m_captureSession;
    QVideoSink *m_videoSink;
    QStringList m_cameraList;
    QString m_currentPath;
    QString m_frame;
    QList<QCameraDevice> m_cameraDevices;
};

#endif
