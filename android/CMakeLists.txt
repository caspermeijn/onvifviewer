# Copyright (C) 2019 Casper Meijn <casper@meijn.net>
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

set(ANDROID_APK_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

# Android manifest
math(EXPR DEFAULT_ANDROID_VERSION_CODE "${PROJECT_VERSION_MAJOR} * 10000 + ${PROJECT_VERSION_MINOR} * 100")  
set(ANDROID_VERSION_CODE "${DEFAULT_ANDROID_VERSION_CODE}" CACHE STRING "The version code to be used when building for Android")
configure_file(AndroidManifest.xml.in ${ANDROID_APK_DIR}/AndroidManifest.xml)

# resources
configure_file(res ${ANDROID_APK_DIR}/ COPYONLY)
