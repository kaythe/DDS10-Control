import QtQuick 2.14
import QtQuick.Window 2.14

import QtQuick.Controls 2.14

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("DDS10 Control")

    Button {
        anchors.centerIn: parent

        text: qsTr("Connect to signal generator")

        enabled: dds ? (!dds.connected) : false

        onClicked: {
            if (dds)
                dds.connectToDevice();
        }
    }
}
