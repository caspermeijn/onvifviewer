# Copyright (C) 2018-2019 Casper Meijn <casper@meijn.net>
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

# desktop file
set(_desktopTemplate net.meijn.onvifviewer.desktop.in)
set(_desktopFile ${CMAKE_CURRENT_BINARY_DIR}/net.meijn.onvifviewer.desktop)
add_custom_command(
    OUTPUT ${_desktopFile}
    COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} --check --use-fuzzy --desktop --template ${_desktopTemplate} -o ${_desktopFile} -d ../po/
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${_desktopTemplate} translations)
add_custom_target(desktop-file ALL DEPENDS ${_desktopFile})
install(FILES ${_desktopFile} DESTINATION ${KDE_INSTALL_APPDIR})

find_program(DESKTOP_FILE_VALIDATE_EXECUTABLE desktop-file-validate)
if(DESKTOP_FILE_VALIDATE_EXECUTABLE)
    add_test(NAME validate-desktop-file COMMAND ${DESKTOP_FILE_VALIDATE_EXECUTABLE} --warn-kde ${_desktopFile})
endif()

# appstream metadata
set(_appDataTemplate net.meijn.onvifviewer.appdata.xml.in)
set(_appDataFile ${CMAKE_CURRENT_BINARY_DIR}/net.meijn.onvifviewer.appdata.xml)
add_custom_command(
    OUTPUT ${_appDataFile}
    COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} --check --use-fuzzy --xml --template ${_appDataTemplate} -o ${_appDataFile} -d ../po/
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${_appDataTemplate} translations)
add_custom_target(app-data ALL DEPENDS ${_appDataFile})
install(FILES ${_appDataFile} DESTINATION ${KDE_INSTALL_METAINFODIR})

find_program(APPSTREAM_UTIL_EXECUTABLE appstream-util)
if(APPSTREAM_UTIL_EXECUTABLE)
    add_test(NAME validate-appdata COMMAND ${APPSTREAM_UTIL_EXECUTABLE} validate-strict ${_appDataFile})
endif()

# icons
install(FILES ApplicationIcon.svg DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps RENAME net.meijn.onvifviewer.svg)
foreach(_iconSize 16 22 24 32 48 64 128 256)
  install(FILES ApplicationIcon-${_iconSize}.png DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/${_iconSize}x${_iconSize}/apps RENAME net.meijn.onvifviewer.png)
endforeach()
