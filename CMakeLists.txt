cmake_minimum_required(VERSION 3.16)

project(VideoRecorder_ALPHA VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick Multimedia Widgets MultimediaWidgets)

qt_standard_project_setup()

qt_add_executable(appVideoRecorder_ALPHA
    main.cpp
)

qt_add_qml_module(appVideoRecorder_ALPHA
    URI VideoRecorder_ALPHA
    VERSION 1.0
    QML_FILES Main.qml
    SOURCES
    SOURCES
    SOURCES videorecorder.h videorecorder.cpp
    SOURCES camerarecorder.h camerarecorder.cpp
    RESOURCES
    SOURCES
    SOURCES imagecapture.h imagecapture.cpp
    QML_FILES
    SOURCES
    SOURCES utils.h utils.cpp
    SOURCES
    QML_FILES

)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appVideoRecorder_ALPHA PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appVideoRecorder_ALPHA
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appVideoRecorder_ALPHA
    PRIVATE Qt6::Quick Qt6::Multimedia Qt6::Widgets Qt6::MultimediaWidgets
)

include(GNUInstallDirs)
install(TARGETS appVideoRecorder_ALPHA
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
