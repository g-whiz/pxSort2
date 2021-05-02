set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

#TODO: move these to a .gitignored file
set(QT_DIR /home/gpg/Qt/5.15.1/gcc_64/lib/cmake/Qt5)
set(Qt5_DIR /home/gpg/Qt/5.15.1/gcc_64/lib/cmake/Qt5)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

add_executable(qt-app-poc
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
        )

add_dependencies(qt-app-poc pxsort)
file(COPY "../../pxsort" DESTINATION "./")
target_include_directories(qt-app-poc PUBLIC "./pxsort")
target_link_libraries(qt-app-poc PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${OpenCV_LIBS} pxsort)
