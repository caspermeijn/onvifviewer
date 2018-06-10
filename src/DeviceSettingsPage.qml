import QtQuick 2.6
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2
import org.kde.kirigami 2.3 as Kirigami
import net.meijn.onvifviewer 1.0
import QtQuick.Controls.Material 2.3

Kirigami.ScrollablePage {
    property bool hasConnectionSettingsChanged: false
    property bool hasOtherSettingsChanged: false

    title: "Settings"
    objectName: "settingsPage"

    onActiveFocusChanged: {
        console.log("onActiveFocusChanged", activeFocus, activeFocusItem)
        if(!activeFocus) {
            if(hasConnectionSettingsChanged || hasOtherSettingsChanged) {
                if(hasConnectionSettingsChanged) {
                    selectedDevice.reconnectToDevice()
                }
                deviceManager.saveDevices()
                hasChanged = false
                hasConnectionSettingsChanged = false;
            }
        }
    }

    property OnvifDevice selectedDevice: deviceManager.at(selectedIndex)

    ColumnLayout {
        spacing: Kirigami.Units.gridUnit

        // TODO: Figure out why this FormLayout is broken if the Style=Default in qtquickcontrols2.conf and work correct if Style=Material
        Kirigami.FormLayout {
            id: layout
            Layout.fillWidth: true

            Kirigami.Separator {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: "Connection settings"
            }
            TextField {
                Kirigami.FormData.label: "Camera name:"
                placeholderText: qsTr("eg. Backyard")
                text: selectedDevice.deviceName
                onTextEdited: {
                    hasOtherSettingsChanged = true
                    selectedDevice.deviceName = text
                }
            }
            TextField {
                Kirigami.FormData.label: "Hostname:"
                placeholderText: qsTr("eg. ipcam.local or 192.168.0.12")
                text: selectedDevice.hostName
                onTextEdited: {
                    hasConnectionSettingsChanged = true
                    selectedDevice.hostName = text
                }
            }
            TextField {
                Kirigami.FormData.label: "Username:"
                text: selectedDevice.userName
                onTextEdited: {
                    hasConnectionSettingsChanged = true
                    selectedDevice.userName = text
                }
            }
            TextField {
                Kirigami.FormData.label: "Password:"
                echoMode: TextInput.Password
                text: selectedDevice.password
                onTextEdited: {
                    hasConnectionSettingsChanged = true
                    selectedDevice.password = text
                }
            }
            Kirigami.Separator {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: "Camera properties"
            }
            Switch {
                Kirigami.FormData.label: "Enable camera movement fix"
                checked: selectedDevice.preferContinuousMove
                onCheckedChanged: {
                    hasOtherSettingsChanged = true
                    selectedDevice.preferContinuousMove = checked
                }
            }
        }
        Button {
            text: "Remove camera"
            onClicked: {
                pageStack.pop();
                deviceManager.removeDevice(selectedIndex)
                deviceManager.saveDevices()
            }
            Layout.fillWidth: true
            Material.background: Material.Red
        }
    }
}
