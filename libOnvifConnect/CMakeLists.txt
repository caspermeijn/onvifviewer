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

cmake_minimum_required(VERSION 3.0)

project(libOnvifConnect VERSION 0.1)

include(GenerateExportHeader)

include(ECMGenerateHeaders)
include(ECMQtDeclareLoggingCategory)

ecm_setup_version(PROJECT
    VARIABLE_PREFIX ONVIF_CONNECT
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/OnvifConnectVersion.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/OnvifConnectConfigVersion.cmake"
    SOVERSION 0)


# if (BUILD_TESTING)
#     add_subdirectory(autotests)
#     add_subdirectory(tests)
# endif()

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/OnvifConnect")

include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/OnvifConnectConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/OnvifConnectConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/OnvifConnectVersion.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}
        COMPONENT Devel)

install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/OnvifConnectConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/OnvifConnectConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

install(EXPORT OnvifConnectTargets
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE OnvifConnectTargets.cmake
        NAMESPACE OnvifConnect::)

install(FILES onvif_connect.categories DESTINATION ${KDE_INSTALL_CONFDIR})

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(GLOBAL_KSWSDL2CPP_OPTION)
option(KDSOAP_SUPPORTS_USE_LOCAL_FILES_ONLY "Build using the -use-local-files-only option of KDSoap; disable this for older KDSoap versions" ON)
if(KDSOAP_SUPPORTS_USE_LOCAL_FILES_ONLY)
    get_filename_component(KDWSDL_IMPORT_PATH ../3rdparty/wsdl/ ABSOLUTE)
    set(GLOBAL_KSWSDL2CPP_OPTION -import-path ${KDWSDL_IMPORT_PATH} -use-local-files-only)
endif()

set(KSWSDL2CPP_OPTION -namespace OnvifSoapDevicemgmt ${GLOBAL_KSWSDL2CPP_OPTION})
KDSOAP_GENERATE_WSDL(libOnvifConnect_WSDL ../3rdparty/wsdl/www.onvif.org/ver10/device/wsdl/devicemgmt.wsdl)
set(KSWSDL2CPP_OPTION -namespace OnvifSoapMedia ${GLOBAL_KSWSDL2CPP_OPTION})
KDSOAP_GENERATE_WSDL(libOnvifConnect_WSDL ../3rdparty/wsdl/www.onvif.org/ver10/media/wsdl/media.wsdl)
set(KSWSDL2CPP_OPTION -namespace OnvifSoapMedia2 ${GLOBAL_KSWSDL2CPP_OPTION})
#TODO: Fix the media2.wsdl file, as this is not the original file path
KDSOAP_GENERATE_WSDL(libOnvifConnect_WSDL ../3rdparty/wsdl/www.onvif.org/ver20/media/wsdl/media2.wsdl)
set(KSWSDL2CPP_OPTION -namespace OnvifSoapPtz ${GLOBAL_KSWSDL2CPP_OPTION})
KDSOAP_GENERATE_WSDL(libOnvifConnect_WSDL ../3rdparty/wsdl/www.onvif.org/ver20/ptz/wsdl/ptz.wsdl)

set(libOnvifConnect_SRCS
    onvifdeviceservice.cpp
    onvifmediaservice.cpp
    onvifdeviceconnection.cpp
    onvifmedia2service.cpp
    onvifptzservice.cpp
    onvifdeviceinformation.cpp
    onvifmediaprofile.cpp
)

ecm_qt_declare_logging_category(libOnvifConnect_SRCS
    HEADER loggingcategory.h
    IDENTIFIER OnvifConnectLog
    CATEGORY_NAME onvifconnect
    DEFAULT_SEVERITY Warning
)

add_library(libOnvifConnect ${libOnvifConnect_SRCS} ${libOnvifConnect_WSDL})
generate_export_header(libOnvifConnect BASE_NAME OnvifConnect)
add_library(OnvifConnect::libOnvifConnect ALIAS libOnvifConnect)

target_include_directories(libOnvifConnect
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/OnvifConnect>"
)

target_link_libraries(libOnvifConnect
    PUBLIC
        Qt5::Core
        KDSoap::kdsoap
    PRIVATE
        Qt5::Network
)

set_target_properties(libOnvifConnect PROPERTIES
    VERSION     ${ONVIF_CONNECT_VERSION_STRING}
    SOVERSION   ${ONVIF_CONNECT_SOVERSION}
)

ecm_generate_headers(OnvifConnect_HEADERS
    HEADER_NAMES
    OnvifDeviceConnection
    OnvifDeviceService
    OnvifMediaService
    OnvifMedia2Service
    OnvifPtzService

    REQUIRED_HEADERS OnvifConnect_HEADERS
)

install(TARGETS libOnvifConnect
        EXPORT OnvifConnectTargets
        ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/onvif_connect_export.h
            ${KArchive_HEADERS}
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/OnvifConnect
        COMPONENT Devel)
