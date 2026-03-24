cmake_minimum_required(VERSION 3.16)

project(RadarTeachingPlatform VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Svg Charts PrintSupport SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg Charts PrintSupport SerialPort)


set(PROJECT_SOURCES
        main.cpp
        loginwindow.cpp
        loginwindow.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RadarTeachingPlatform
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

        fontManager.cpp
        fontManager.h
        teachingMainWindow.h
        teachingMainWindow.cpp
        resources.qrc
        customMainWindow.h
        mainWindow.h
        customMainWindow.cpp
        mainWindow.cpp
        experimentTypes.h
        collapsibleGroup.h
        collapsibleGroup.cpp
        experimentSidebar.h
        experimentSidebar.cpp
        radarDataModel.h
        radarConfigLoader.h
        sidebarListPanel.h
        sidebarListPanel.cpp
        ExperimentManager.h
        instrumentpanel.h
        instrumentpanel.cpp
        instrumentpanel.h
        instrumentpanel.cpp
        qcustomplot.h
        qcustomplot.cpp
        OscilloScopeWidget.h
        oscilloscopeWidget.cpp
        radarRangingDisplay.h radarRangingDisplay.cpp
        radarDataParser.h radarDataParser.cpp
        serialTestHelper.h serialTestHelper.cpp
        radarDistanceWidget.h
        radarDistanceWidget.cpp

    )

file(COPY ${CMAKE_SOURCE_DIR}/resources
     DESTINATION ${CMAKE_BINARY_DIR})


# Define target properties for Android with Qt 6 as:
#    set_property(TARGET RadarTeachingPlatform APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(RadarTeachingPlatform SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(RadarTeachingPlatform
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(RadarTeachingPlatform PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
    Qt${QT_VERSION_MAJOR}::SerialPort
    Qt${QT_VERSION_MAJOR}::Svg
    Qt${QT_VERSION_MAJOR}::Charts
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.RadarTeachingPlatform)
endif()
set_target_properties(RadarTeachingPlatform PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS RadarTeachingPlatform
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RadarTeachingPlatform)
endif()
