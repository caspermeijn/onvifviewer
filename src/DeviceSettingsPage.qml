/* Copyright (C) 2018 Casper Meijn <casper@meijn.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
