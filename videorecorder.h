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
#include <QTimer>
#include <QBuffer>
#include <QMediaRecorder>
#include <QMediaFormat>
#include <QUrl>

#include "utils.h"

class VideoRecorder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList cameraList READ getCameraList NOTIFY cameraListChanged)
    Q_PROPERTY(QString frame READ getFrame NOTIFY frameChanged)
    Q_PROPERTY(bool isRecording READ isRecording NOTIFY recordingStatusChanged)

public:
    explicit VideoRecorder(QObject *parent = nullptr);
    ~VideoRecorder();

    QStringList getCameraList() const { return tab_camera_list; }
    QString getFrame() const;
    bool isRecording() const { return is_recording; }

    Q_INVOKABLE QString getCurrentPath() const { return Utils::getMediaPath(); }
    Q_INVOKABLE void setCurrentPath(const QString &path) {Utils::setMediaPath(path);};

public slots:
    void setCamera(int index);
    void captureFrame();
    void startStopRecording();

signals:
    void cameraListChanged();
    void frameChanged();
    void recordingStatusChanged(bool is_recording);


private slots:
    void handleFrameChanged(const QVideoFrame &frame);
    void updateFrame();

private:
    void updateCameraList();
    void processAndEmitFrame(const QVideoFrame &frame);
    bool configureMediaRecorder();

    QCamera *ptr_camera;
    QMediaCaptureSession capture_session;
    QVideoSink *ptr_video_sink;
    QStringList tab_camera_list;
    QString frame;
    QList<QCameraDevice> tab_camera_devices;
    QTimer *ptr_frame_timer;
    QVideoFrame last_frame;
    QFile* ptr_video_file;
    bool is_recording;

    QMediaRecorder* ptr_media_recorder;
};

#endif // VIDEORECORDER_H
