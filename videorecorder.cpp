#include "videorecorder.h"
#include <QBuffer>

VideoRecorder::VideoRecorder(QObject *parent)
    : QObject(parent)
    , m_camera(nullptr)
    , m_videoSink(new QVideoSink(this))
    , m_currentPath(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/VideoRecorder")
{
    connect(m_videoSink, &QVideoSink::videoFrameChanged,
            this, &VideoRecorder::handleFrameChanged);

    m_captureSession.setVideoSink(m_videoSink);
    updateCameraList();

    if (!m_cameraDevices.isEmpty()) {
        m_camera = new QCamera(m_cameraDevices.first(), this);
        m_captureSession.setCamera(m_camera);
        m_camera->start();
    }
}

VideoRecorder::~VideoRecorder()
{
    if (m_camera) {
        m_camera->stop();
        delete m_camera;
    }
}

QString VideoRecorder::getFrame() const
{
    return m_frame;
}

void VideoRecorder::handleFrameChanged(const QVideoFrame &frame)
{
    if (!frame.isValid()) return;

    QVideoFrame clonedFrame = frame;
    if (!clonedFrame.map(QVideoFrame::ReadOnly)) return;

    QImage image = clonedFrame.toImage();
    clonedFrame.unmap();

    if (!image.isNull()) {
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG");
        buffer.close();

        QString base64 = QString("data:image/png;base64,") + QString::fromLatin1(byteArray.toBase64());
        if (m_frame != base64) {
            m_frame = base64;
            emit frameChanged();
        }
    }
}

void VideoRecorder::updateCameraList()
{
    m_cameraDevices = QMediaDevices::videoInputs();
    m_cameraList.clear();

    for (const QCameraDevice &device : m_cameraDevices) {
        m_cameraList.append(device.description());
    }

    emit cameraListChanged();
}

void VideoRecorder::setCamera(int index)
{
    if (index >= 0 && index < m_cameraDevices.size()) {
        if (m_camera) {
            m_camera->stop();
            delete m_camera;
        }

        m_camera = new QCamera(m_cameraDevices.at(index), this);
        m_captureSession.setCamera(m_camera);
        m_camera->start();
    }
}

void VideoRecorder::setCurrentPath(const QString &path)
{
    if (m_currentPath != path) {
        m_currentPath = path;
        emit currentPathChanged();
    }
}

void VideoRecorder::captureFrame()
{
    if (!m_videoSink) return;

    QVideoFrame frame = m_videoSink->videoFrame();
    if (!frame.isValid()) return;

    if (!frame.map(QVideoFrame::ReadOnly)) return;

    QImage image = frame.toImage();
    frame.unmap();

    if (image.isNull()) return;

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString filename = m_currentPath + "/capture_" + timestamp + ".png";

    QDir().mkpath(m_currentPath);

    if (image.save(filename)) {
        qDebug() << "Frame saved to:" << filename;
    }
}
