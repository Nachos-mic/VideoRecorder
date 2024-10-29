import QtQuick
import QtQuick.Window
import QtMultimedia
import QtQuick.Controls

Window {
    width: 640
    height: 480
    visible: true

    property string camera_id : ""

    Rectangle{
        id: options
        height: 40
        width: parent.width
        anchors{
            top: parent.top
            left: parent.left
        }
        color: "steelblue"
    }

    CaptureSession {
        camera: Camera {
            cameraDevice: camera_id
            Component.onCompleted: start()
        }
        videoOutput: videoFeed
    }

    VideoOutput {
        id: videoFeed
        anchors{
            top: options.bottom
            left: parent.left
        }
        height: parent.height - options.height
        width: parent.width
    }

    Connections{

        function onDeviceListChanged(id_list){
            camera_id = id_list[0]
        }
    }
}
