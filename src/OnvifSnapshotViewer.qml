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
import QtQuick 2.9

Item {
    property string snapshotUri
    property alias interval: timer.interval
    property real aspectRatio: image1.isReady ? image1.aspectRatio : (image2.isReady ? image2.aspectRatio : 2)

    function isSnapshotAvailable() {
        return image1.status == Image.Ready || image2.status == Image.Ready
    }

    id: item1

    onSnapshotUriChanged: {
        image1.source = ""
        image2.source = ""
        downloadNextImage()
    }
    onVisibleChanged: {
        if(item1.visible)
            downloadNextImage()
    }

    function downloadNextImage() {
        if(snapshotUri)
        {
            if(image1.status == Image.Ready)
            {
                image2.source = snapshotUri
            }
            else
            {
                image1.source = snapshotUri
            }
        }
    }

    Image {
        id: image1
        property bool isReady: status == Image.Ready
        property real aspectRatio: implicitWidth / implicitHeight
        anchors.fill: parent
        visible: false
        fillMode: Image.PreserveAspectFit
        cache: false
        onStatusChanged: {
            if(image1.status == Image.Ready)
            {
                image1.visible = true
                image2.visible = false
                image2.source = ""
                if(item1.visible)
                    timer.start()
            }
        }
    }
    Image {
        id: image2
        property bool isReady: status == Image.Ready
        property real aspectRatio: implicitWidth / implicitHeight
        anchors.fill: parent
        visible: false
        fillMode: Image.PreserveAspectFit
        cache: false
        onStatusChanged: {
            if(image2.status == Image.Ready)
            {
                image2.visible = true
                image1.visible = false
                image1.source = ""
                if(item1.visible)
                    timer.start()
            }
        }
    }

    Timer {
        id: timer
        interval: 1000; running: false; repeat: false;
        onTriggered: {
            downloadNextImage()
        }
    }
}
