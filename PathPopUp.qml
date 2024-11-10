import QtQuick
import QtQuick.Controls

Popup {

        property string path: ""

        id: root
        x: parent.width/2
        y: parent.height/2
        width: 200
        height: 100
        modal: true
        focus: true

        anchors{
            centerIn: parent
        }

        TextField{
            id:pathField
            text: path
        }

        Button{
            id:setButton
            onClicked:{
                cameraUser.setPath(pathField.text)
                pathSetPop.close()
            }
            text:"Set Path"
            anchors {
                top: pathField.bottom
                topMargin: 20
                horizontalCenter: parent.horizontalPadding
            }
        }

        Button{
            id:cancelButton
            onClicked: pathSetPop.close()
            text:"Cancel"
            anchors {
                top: setButton.bottom
                topMargin: 20
                left: setButton.right
                leftMargin: 30
                horizontalCenter: parent.horizontalPadding
            }
        }
}
