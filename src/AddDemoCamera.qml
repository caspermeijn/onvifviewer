/* Copyright (C) 2018 Casper Meijn <casper@meijn.net>
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
import QtQuick.Controls 2.3

Kirigami.ScrollablePage {
    title: i18n("Add demo camera")
    objectName: "addDemoCameraPage"

    ListModel {
        id: demoCameraModel
        ListElement {
            deviceName: "Demo Norway"
            hostName: "79.160.18.23:10000"
            userName: ""
            password: ""
            preferredVideoStreamProtocol: "RtspOverHttp"
        }
        ListElement {
            deviceName: "Demo Zurich"
            hostName: "213.173.165.16:90"
            userName: ""
            password: ""
        }
        ListElement {
            deviceName: "Demo frontdoor"
            hostName: "84.171.95.10:50001"
            userName: "service"
            password: "service"
        }
    }

    ListView {
        anchors.fill: parent
        model: demoCameraModel
        delegate: Button {
            text: deviceName
            onClicked: {
                selectedIndex = deviceManager.appendDevice()
                var newDevice = deviceManager.at(selectedIndex)
                newDevice.deviceName = deviceName;
                newDevice.hostName = hostName;
                newDevice.userName = userName ? userName : "";
                newDevice.password = password ? password : "";
                newDevice.preferredVideoStreamProtocol = preferredVideoStreamProtocol ? preferredVideoStreamProtocol : "";
                newDevice.connectToDevice();
                deviceManager.saveDevices()

                pageStack.pop();
                pageStack.push(deviceViewerComponent);
            }
            width: parent.width
        }
    }
}
