cmake_minimum_required(VERSION 3.16)

set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt")

project(Sky114DebugApplication VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PahoMqttCpp REQUIRED)

#find_package(Qt6 REQUIRED COMPONENTS Quick Core Gui Qml Graphs Mqtt)
find_package(Qt6 REQUIRED COMPONENTS Quick Core Gui Qml Graphs)

qt_standard_project_setup(REQUIRES 6.7)

qt_add_executable(appSky114DebugApplication
    main.cpp
)

qt_add_qml_module(appSky114DebugApplication
    URI Sky114DebugApplication
    QML_FILES
        Main.qml
        SOURCES mqtttransceiver.h mqtttransceiver.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appSky114DebugApplication PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appSky114DebugApplication
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appSky114DebugApplication
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Graphs
    #Qt6::Mqtt
    PahoMqttCpp::paho-mqttpp3
)

include(GNUInstallDirs)
install(TARGETS appSky114DebugApplication
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
