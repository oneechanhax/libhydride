
# Libhydride: A transparent drawable GL layer for your desktop! 
# Copyright (C) 2022 Rebekah Rowe
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
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.11)

project(hydride LANGUAGES C VERSION 1.0.0)

find_package(X11 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

add_library(hydride SHARED "src/drawglx.c" "src/hydride.c")

target_include_directories(hydride PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(hydride PUBLIC ${GLEW_LIBRARIES} m rt pthread ${X11_X11_LIB} ${X11_Xext_LIB} ${X11_Xfixes_LIB})

install(TARGETS ${PROJECT_NAME} DESTINATION "lib/")
install(FILES "include/hydride.h" DESTINATION "include/")
install(FILES "include/xoverlay.h" DESTINATION "include/")
