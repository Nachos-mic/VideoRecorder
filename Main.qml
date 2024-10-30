import QtQuick
import QtQuick.Window
import QtMultimedia
import QtQuick.Controls

Window {
    width: 640
    height: 480
    visible: true

    property string camera_id : ""
    property var camera_name_list : []
    property var camera_id_list: []
    property var cameraList: []


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

            id: cameraBox

            model: camera_name_list
            height: parent.height
            width: parent.width/6
            onCurrentIndexChanged: changeCamera()
        }
    }

    CaptureSession {
        camera: Camera {
            id: camera
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

    MediaDevices {
        id: mediaDevices
    }

    function changeCamera(){
        for (let i = 0; i < camera_id_list.length; ++i) {
            let device = mediaDevices.videoInputs[i]
            console.log(device.id)
            console.log(camera_id_list[cameraBox.currentIndex])
            if (device.id == camera_id_list[cameraBox.currentIndex]) {
                camera.cameraDevice = device
                console.log(device.id)
                break
            }
        }
    }

    Connections{

        target: cameraUser

        function onDeviceListChanged(id_list , name_list){
            camera_id_list = id_list
            camera_name_list = name_list;
        }


    }
}
