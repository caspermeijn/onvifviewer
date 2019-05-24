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
cmake_policy(SET CMP0063 NEW)

include(GenerateExportHeader)

include(ECMGenerateHeaders)

# if (BUILD_TESTING)
#     add_subdirectory(autotests)
#     add_subdirectory(tests)
# endif()

set(GLOBAL_KSWSDL2CPP_OPTION)
if(KDSoap_VERSION GREATER_EQUAL 1.8.0)
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

add_library(libOnvifConnect STATIC ${libOnvifConnect_SRCS} ${libOnvifConnect_WSDL})
generate_export_header(libOnvifConnect BASE_NAME OnvifConnect)

target_link_libraries(libOnvifConnect
    PUBLIC
        Qt5::Core
        KDSoap::kdsoap
    PRIVATE
        Qt5::Network
)
