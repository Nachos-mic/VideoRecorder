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
    QList<QCameraDevice> getDeviceList();\

private:
    QList<QString> tab_id_list;

signals:

    void deviceListChanged(QList<QString> tab_camera_id_list);
};

#endif // DEVICECHECK_H
