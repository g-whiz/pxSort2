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

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

set(TS_FILES pxsort2_en_CA.xts)

if(ANDROID)
    add_library(pxsort2 SHARED
            main.cpp
            mainwindow.cpp
            mainwindow.h
            mainwindow.ui
            ${TS_FILES}
            )
else()
    add_executable(pxsort2
            main.cpp
            mainwindow.cpp
            mainwindow.h
            mainwindow.ui
            ${TS_FILES}
            )
endif()

target_link_libraries(pxsort2 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
