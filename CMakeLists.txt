cmake_minimum_required(VERSION 3.16)

project(BDFDAdW VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Core)

# 1. SETUP OPENCV
# Ensure this path points to the folder containing 'OpenCVConfig.cmake'
# If you downloaded pre-built binaries, it is usually in /x64/mingw/lib
set(OpenCV_DIR "C:/opencv/x64/mingw/lib")
find_package(OpenCV REQUIRED)

qt_standard_project_setup(REQUIRES 6.8)

# 2. CREATE EXECUTABLE (Must come before linking!)
qt_add_executable(appBDFDAdW
    main.cpp
    windows_icon.rc
)

qt_add_qml_module(appBDFDAdW
    URI BDFDAdW
    VERSION 1.0
    QML_FILES
        Main.qml
    SOURCES
        backend.h
        backend.cpp
)

# 3. LINK LIBRARIES
# Now that 'appBDFDAdW' exists, we can link to it.
target_link_libraries(appBDFDAdW PRIVATE
    Qt6::Quick
    Qt6::Core
    ${OpenCV_LIBS} # <--- OpenCV Linked Here
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
set_target_properties(appBDFDAdW PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS appBDFDAdW
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
