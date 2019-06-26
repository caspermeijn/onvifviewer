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

file(GLOB PO_FILES *.po)
SET(MO_FILES)
foreach(_poFile ${PO_FILES})
    get_filename_component(_langCode ${_poFile} NAME_WE)
    set(_moFile ${CMAKE_CURRENT_BINARY_DIR}/${_langCode}.mo)
    add_custom_command(
        OUTPUT ${_moFile}
        COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} --check --use-fuzzy -o ${_langCode}.mo ${_poFile}
        DEPENDS ${_poFile})
    install(FILES ${_moFile} DESTINATION ${LOCALE_INSTALL_DIR}/${_langCode}/LC_MESSAGES/ RENAME ${PROJECT_NAME}.mo)
    list(APPEND MO_FILES ${_moFile})
endforeach()
add_custom_target(translations ALL DEPENDS ${MO_FILES})
