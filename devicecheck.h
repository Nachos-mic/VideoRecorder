#ifndef DEVICECHECK_H
#define DEVICECHECK_H
#include <QtMultimedia>
#include <QCamera>
#include <QCameraDevice>
#include <QDebug>
#include <QTimer>


class DeviceCheck : public QObject {
    Q_OBJECT
public:
    DeviceCheck();
    QList<QCameraDevice> getDeviceList();
};

#endif // DEVICECHECK_H
