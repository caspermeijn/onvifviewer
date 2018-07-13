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
import QtMultimedia 5.9
import QtQuick 2.9

Item {
    id: viewer

    property alias streamUri: video.source

    function isStreamAvailable() {
        return video.playbackState === MediaPlayer.PlayingState && video.hasVideo && video.source
    }

    onVisibleChanged: {
        if(viewer.visible)
            video.play()
        else
            video.stop()
    }

    Video {
        id: video
        muted: true
        anchors.fill: parent

        autoLoad: true
        autoPlay: true
    }
}
