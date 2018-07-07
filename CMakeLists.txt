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

project(onvifviewer VERSION "0.1")
SET(CMAKE_CXX_STANDARD 11)
set(QT_MIN_VERSION "5.9.0")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Core Svg Qml Quick QuickControls2 Xml)
find_package(KF5Kirigami2 REQUIRED)
find_package(Qt5Widgets)

# Set ROOT_PATH_MODE to enable KDSoap to find the host wsdl compiler
#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)

#find_package(KDSoap REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
find_package(KDSoap REQUIRED)

find_package(ECM 5.38.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
set(ENABLE_CLAZY TRUE)
include(ECMQMLModules)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

#TODO: figure out whye these qml module checks fail for gitlab-ci
#ecm_find_qmlmodule(QtQuick 2.9 REQUIRED)
#ecm_find_qmlmodule(QtQuick.Controls 1.4 REQUIRED)
##TODO: figure out why QtQuick.Controls 2.0 is not foun
##ecm_find_qmlmodule(QtQuick.Controls 2.0 REQUIRED)
#ecm_find_qmlmodule(QtQuick.Layouts 1.3 REQUIRED)
#ecm_find_qmlmodule(QtQml.Models 2.1 REQUIRED)
#ecm_find_qmlmodule(org.kde.kirigami 2.0 REQUIRED)

include(FeatureSummary)

add_subdirectory(src)
add_subdirectory(desktop)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
