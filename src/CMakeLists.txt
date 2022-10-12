include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(qt_tick_DBUS_XML qt_tick.QtTick.xml)
set(qt_tick_SRC
    qttick_dbus.cpp
    qttick_event_listener.cpp
)

qt5_wrap_cpp(qt_tick_SRC
    qttick_dbus.h
    qttick_event_listener.h
)

configure_file(${qt_tick_DBUS_XML} ${CMAKE_CURRENT_BINARY_DIR}/${qt_tick_DBUS_XML} COPYONLY)

qt5_add_dbus_adaptor(qt_tick_DBUS_SRC
    ${CMAKE_CURRENT_BINARY_DIR}/${qt_tick_DBUS_XML}
    qttick_dbus.h
    QtTickDBus
)

add_library(qt_tick SHARED
    qttick_preloader.cpp
    ${qt_tick_SRC}
    ${qt_tick_DBUS_SRC}
)

target_link_libraries(qt_tick
    PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    Qt5::DBus
)
