import QtQuick 2.6
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2
import org.kde.kirigami 2.3 as Kirigami
import net.meijn.onvifviewer 1.0

Kirigami.ScrollablePage {
    property bool hasChanged: false

    title: "Settings"
    objectName: "settingsPage"

    onActiveFocusChanged: {
        console.log("onActiveFocusChanged", activeFocus, activeFocusItem)
        if(!activeFocus) {
            if(hasChanged) {
                selectedDevice.reconnectToDevice()
                deviceManager.saveDevices()
                hasChanged = false
            }
        }
    }

    property OnvifDevice selectedDevice: deviceManager.at(selectedIndex)

    Kirigami.FormLayout {
        id: layout

        Kirigami.Separator {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: selectedDevice.deviceName
        }
        TextField {
            Kirigami.FormData.label: "Camera name:"
            placeholderText: qsTr("eg. Backyard")
            text: selectedDevice.deviceName
            onTextEdited: {
                hasChanged = true
                selectedDevice.deviceName = text
            }
        }
        TextField {
            Kirigami.FormData.label: "Hostname:"
            placeholderText: qsTr("eg. ipcam.local or 192.168.0.12")
            text: selectedDevice.hostName
            onTextEdited: {
                hasChanged = true
                selectedDevice.hostName = text
            }
        }
        TextField {
            Kirigami.FormData.label: "Username:"
            text: selectedDevice.userName
            onTextEdited: {
                hasChanged = true
                selectedDevice.userName = text
            }
        }
        TextField {
            Kirigami.FormData.label: "Password:"
            echoMode: TextInput.Password
            text: selectedDevice.password
            onTextEdited: {
                hasChanged = true
                selectedDevice.password = text
            }
        }
    }
}
