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
import QtQuick 2.0
import QtQuick.Controls 2.2 as QQC2
import QtMultimedia 5.9

Item {
    property alias snapshotUri: snapshotViewer.snapshotUri
    property alias snapshotInterval: snapshotViewer.interval
    property alias streamUri: streamViewer.streamUri
    property real aspectRatio: snapshotViewer.visible ? snapshotViewer.aspectRatio : 2

    QQC2.Label {
        id: loadingLabel
        text: qsTr("Loading...")
        visible: !snapshotViewer.visible && !streamViewer.visible
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.fill: parent
        font.pixelSize: 12
    }

    OnvifSnapshotViewer {
        id: snapshotViewer
        anchors.fill: parent
        visible: snapshotViewer.isSnapshotAvailable() && !streamViewer.visible
    }

    OnvifStreamViewer {
        id: streamViewer
        anchors.fill: parent
        visible: streamViewer.isStreamAvailable()
    }
}
