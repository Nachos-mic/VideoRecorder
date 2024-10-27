import QtQuick 2.12
import QtQuick.Window
import QtMultimedia

Window {
    width: 640
    height: 480
    visible: true
    Camera {
        id: camera
        Component.onCompleted: cameraHelper.qcamera = camera
    }
    VideoOutput {
        // source: camera
        focus : visible
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent;
            onClicked: cameraHelper.capture();
        }
    }
}
