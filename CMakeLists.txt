cmake_minimum_required(VERSION 3.13)
project(NVMRVideoSender VERSION 1.0.0)

set( CMAKE_CXX_STANDARD 17 )
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
)

include(FindPkgConfig)
pkg_check_modules( gstreamer REQUIRED gstreamer-1.0 )
pkg_check_modules( log4cxx REQUIRED liblog4cxx )
pkg_check_modules( nvmr-json REQUIRED nvmr-json )
pkg_check_modules( dbus-cxx REQUIRED dbus-cxx-qt-2.0 )

add_executable( NVMRVideoSender ${headers} ${sources} )
target_link_libraries( NVMRVideoSender
    Qt5::Core
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
