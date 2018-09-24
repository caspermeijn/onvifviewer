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
import net.meijn.onvifviewer 1.0
import org.kde.kirigami 2.3 as Kirigami
import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3

Kirigami.ScrollablePage {
    property bool hasConnectionSettingsChanged: false
    property bool hasOtherSettingsChanged: false
    property bool isNewDevice: false

    title: isNewDevice ? i18n("New manual device") : i18n("Device settings")
    objectName: "settingsPage"

    onIsCurrentPageChanged: {
        if(!isCurrentPage) {
            if(hasConnectionSettingsChanged || hasOtherSettingsChanged) {
                if(hasConnectionSettingsChanged) {
                    selectedDevice.reconnectToDevice()
                }
                deviceManager.saveDevices()
                hasConnectionSettingsChanged = false;
                hasOtherSettingsChanged = false
            }
            isNewDevice = false
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
                Kirigami.FormData.label: i18n("Connection settings")
            }
            TextField {
                Kirigami.FormData.label: i18n("Camera name:")
                placeholderText: i18n("eg. Backyard")
                text: selectedDevice && selectedDevice.deviceName
                onTextEdited: {
                    hasOtherSettingsChanged = true
                    selectedDevice.deviceName = text
                }
            }
            TextField {
                Kirigami.FormData.label: i18n("Hostname:")
                placeholderText: i18n("eg. ipcam.local or 192.168.0.12")
                text: selectedDevice && selectedDevice.hostName
                onTextEdited: {
                    hasConnectionSettingsChanged = true
                    selectedDevice.hostName = text
                }
            }
            TextField {
                Kirigami.FormData.label: i18n("Username:")
                text: selectedDevice && selectedDevice.userName
                onTextEdited: {
                    hasConnectionSettingsChanged = true
                    selectedDevice.userName = text
                }
            }
            TextField {
                Kirigami.FormData.label: i18n("Password:")
                echoMode: TextInput.Password
                text: selectedDevice && selectedDevice.password
                onTextEdited: {
                    hasConnectionSettingsChanged = true
                    selectedDevice.password = text
                }
            }
            Kirigami.Separator {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: i18n("Camera properties")
            }
            Switch {
                Kirigami.FormData.label: i18n("Enable camera movement fix")
                checked: selectedDevice && selectedDevice.preferContinuousMove
                onCheckedChanged: {
                    hasOtherSettingsChanged = true
                    selectedDevice.preferContinuousMove = checked
                }
            }
        }
        Button {
            text: i18n("Remove camera")
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
