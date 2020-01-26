# Copyright (C) 2019 Casper Meijn <casper@meijn.net>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.7)

set(_appDataFile ${CMAKE_CURRENT_BINARY_DIR}/../desktop/net.meijn.onvifviewer.appdata.xml)
set(_fastlaneMetadata ${CMAKE_CURRENT_SOURCE_DIR}/metadata/android/)
add_custom_command(
    OUTPUT ${_fastlaneMetadata}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/convert_appstream_to_fastlane.py --appstream ${_appDataFile} --fastlane ${_fastlaneMetadata}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS app-data ${_appDataFile})
add_custom_target(fastlane-metadata DEPENDS ${_fastlaneMetadata})
