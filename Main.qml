import QtQuick
import QtQuick.Window
import QtMultimedia
import QtQuick.Controls

Window {
    width: 640
    height: 480
    visible: true

    property string camera_id : ""
    property var camera_id_list : [""]

    Rectangle{
        id: options
        height: 40
        width: parent.width
        anchors{
            top: parent.top
            left: parent.left
        }
        color: "steelblue"

        ComboBox {

            model: camera_id_list
            width: parent.width/6
            onCurrentIndexChanged: console.debug(camera_id_list[currentIndex])
        }
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

        target: cameraUser

        function onDeviceListChanged(id_list){
            camera_id = id_list[0]
            console.log(camera_id)
            camera_id_list = id_list;
        }
    }
}
