#ifndef VIDEORECORDER_H
#define VIDEORECORDER_H

#include <QtMultimedia>
#include <QCamera>
#include <QCameraDevice>
#include <QDebug>
#include <QTimer>
#include <QMediaRecorder>
#include <QtCore>
#include "imagecapture.h"
//#include "videocapture.h"

class VideoRecorder : public QObject {
    Q_OBJECT
    Q_PROPERTY(QCamera* camera_device READ getCamera WRITE setCamera NOTIFY cameraChanged)

public:
    explicit VideoRecorder(QObject *parent = nullptr);
    ~VideoRecorder();
    ImageCapture* imageCaptureManager;
    QList<QCameraDevice> getDeviceList();
    QCamera* getCamera();
    void setCamera(QCamera* camera);
    // Q_INVOKABLE void createCamera(const QString& deviceId);
    QList<QString> tab_id_list;
    QStringList tab_camera_names_list;

private:
    QCamera* camera_device;
    int camera_list_size = 0;

public slots:
    void createCamera(const QString& deviceId);
    void captureFrame();
    void setPath(QString path);

signals:
    void deviceListChanged(QList<QString> ids, QStringList names);
    void cameraChanged();
    void frameCapture(QCamera* camera);
    void pathChanged(QString path);
};
#endif // VIDEORECORDER_H
