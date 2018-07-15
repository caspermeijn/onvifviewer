# Copyright (C) 2018 Casper Meijn <casper@meijn.net>
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

project(onvifviewer VERSION "0.5")
SET(CMAKE_CXX_STANDARD 11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

get_filename_component(BREEZEICONS_DIR 3rdparty/breeze-icons/ ABSOLUTE)
if (NOT EXISTS ${BREEZEICONS_DIR})
    message(FATAL_ERROR "Breeze-icons is missing, please checkout a copy at: \"${BREEZEICONS_DIR}\"")
endif()

find_package(ECM 5.44.0 REQUIRED NO_MODULE)
find_package(KDSoap 1.7.0 REQUIRED)
find_package(KF5Kirigami2 5.44.0 REQUIRED)
find_package(Qt5 5.10.0 REQUIRED NO_MODULE COMPONENTS Core Svg Qml Quick QuickControls2 Xml)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
set(ENABLE_CLAZY TRUE)
include(ECMQMLModules)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

##TODO: figure out whye these qml module checks fail for gitlab-ci
#ecm_find_qmlmodule(org.kde.kirigami 2.3 REQUIRED)
#ecm_find_qmlmodule(QtMultimedia 5.9 REQUIRED)
#ecm_find_qmlmodule(QtQml.Models 2.1 REQUIRED)
#ecm_find_qmlmodule(QtQuick 2.9 REQUIRED)
#ecm_find_qmlmodule(QtQuick.Controls 1.4 REQUIRED)
##TODO: figure out why QtQuick.Controls 2.3 is not found
##ecm_find_qmlmodule(QtQuick.Controls 2.3 REQUIRED)
##ecm_find_qmlmodule(QtQuick.Controls.Material 2.3 REQUIRED)
#ecm_find_qmlmodule(QtQuick.Layouts 1.3 REQUIRED)

include(FeatureSummary)

add_subdirectory(src)
add_subdirectory(desktop)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
