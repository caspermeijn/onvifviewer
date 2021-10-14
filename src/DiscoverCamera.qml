/* Copyright (C) 2018-2019 Casper Meijn <casper@meijn.net>
 * SPDX-License-Identifier: GPL-3.0-or-later
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
import org.kde.kirigami 2.6 as Kirigami
import QtQuick 2.9
import QtQuick.Controls 2.3 as Controls
import QtQuick.Layouts 1.3

Kirigami.ScrollablePage {
    id: pageDiscoverCamera
    title: i18n("Discover camera")
    objectName: "discoverCameraPage"

    ColumnLayout {
        width: pageDiscoverCamera.width
        height: pageDiscoverCamera.height

        Controls.Label {
            text: i18n("No camera discovered in the local network.")
            visible: deviceDiscover.matchList.length === 0
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Controls.Label {
            text: i18n("Click on a discovered camera to add it:")
            visible: deviceDiscover.matchList.length !== 0
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        ListView {
            visible: deviceDiscover.matchList.length !== 0
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: deviceDiscover.matchList
            delegate: Kirigami.BasicListItem {
                icon: "camera-video"
                label: modelData.name + "/" + modelData.hardware
                onClicked: {
                    selectedIndex = deviceManager.appendDevice()
                    var newDevice = deviceManager.at(selectedIndex)
                    newDevice.deviceName = modelData.name;
                    console.log(modelData.xAddr[0])
                    newDevice.hostName = modelData.host[0];
                    newDevice.connectToDevice();
                    deviceManager.saveDevices()

                    pageStack.pop();
                    pageStack.push(settingsComponent);
                }
                width: parent.width
            }
        }
    }
}
