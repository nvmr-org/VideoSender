cmake_minimum_required(VERSION 3.13)
project(NVMRVideoSender VERSION 1.0.0)

set( CMAKE_CXX_STANDARD 17 )

include(FeatureSummary)
find_package(ECM 5.54.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

# need KDEInstallDirs for qmake/moc to work correctly
include(KDEInstallDirs)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
add_compile_definitions(QT_NO_KEYWORDS)

include(GNUInstallDirs)

find_package(Qt5 COMPONENTS Core WebSockets REQUIRED)

set( headers
    videosender.h
    configurationserver.h
    avahi-dbus/EntryGroupProxy.h
    avahi-dbus/ServerProxy.h
    avahi-dbus/org_freedesktop_Avahi_ServerProxy.h
    avahi-dbus/org_freedesktop_Avahi_Server2Proxy.h
    avahi-dbus/org_freedesktop_Avahi_EntryGroupProxy.h
    avahi.h
    clientconnection.h
)

set( sources
    main.cpp
    videosender.cpp
    configurationserver.cpp
    avahi-dbus/EntryGroupProxy.cpp
    avahi-dbus/ServerProxy.cpp
    avahi-dbus/org_freedesktop_Avahi_ServerProxy.cpp
    avahi-dbus/org_freedesktop_Avahi_Server2Proxy.cpp
    avahi-dbus/org_freedesktop_Avahi_EntryGroupProxy.cpp
    avahi.cpp
    clientconnection.cpp
)

include(FindPkgConfig)
pkg_check_modules( gstreamer REQUIRED gstreamer-rtsp-server-1.0 )
pkg_check_modules( log4cxx REQUIRED liblog4cxx )
pkg_check_modules( nvmr-json REQUIRED nvmr-json )
pkg_check_modules( dbus-cxx REQUIRED dbus-cxx-qt-2.0 )

add_executable( NVMRVideoSender ${headers} ${sources} )
target_link_libraries( NVMRVideoSender
    Qt5::Core
    Qt5::WebSockets
    ${gstreamer_LIBRARIES}
    ${log4cxx_LIBRARIES}
    ${nvmr-json_LIBRARIES}
    ${dbus-cxx_LIBRARIES}
)
target_include_directories( NVMRVideoSender PRIVATE
    ${gstreamer_INCLUDE_DIRS}
    ${log4cxx_INCLUDE_DIRS}
    ${nvmr-json_INCLUDE_DIRS}
    ${dbus-cxx_INCLUDE_DIRS}
)

#
# RTSP helper application
#
add_executable( NVMRVideoSenderRTSPHelper video-sender-rtsp-helper.c )
target_link_libraries( NVMRVideoSenderRTSPHelper
    ${gstreamer_LIBRARIES}
)
target_include_directories( NVMRVideoSenderRTSPHelper PRIVATE
    ${gstreamer_INCLUDE_DIRS}
)

#
# install information
#
install( TARGETS NVMRVideoSender NVMRVideoSenderRTSPHelper
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install( FILES videosender.service
    DESTINATION /lib/systemd/system
)
