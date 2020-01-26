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
import QtMultimedia 5.9
import QtQuick 2.9
import QtQuick.Controls 2.3 as QQC2
import net.meijn.onvifviewer 1.0

Item {
    property OnvifDevice camera
    property int snapshotInterval: 1000
    property real aspectRatio: snapshotViewer.aspectRatio
    property bool loadStream: true

    QQC2.Label {
        id: loadingLabel
        text: i18n("Loading…")
        visible: !snapshotViewer.visible && !streamViewer.visible
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.fill: parent
    }

    OnvifSnapshotViewer {
        id: snapshotViewer
        anchors.fill: parent
        interval: streamViewer.visible ? -1 : streamViewer.hasError ? 0 : snapshotInterval
        downloader: camera ? camera.snapshotDownloader : null
        visible: snapshotViewer.isSnapShotAvailable && !streamViewer.visible
    }

    OnvifStreamViewer {
        id: streamViewer
        anchors.fill: parent
        streamUri: (loadStream && camera) ? camera.streamUri : ""
        visible: streamViewer.isStreamAvailable()
    }
}
