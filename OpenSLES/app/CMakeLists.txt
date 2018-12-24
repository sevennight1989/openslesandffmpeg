# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
set(distribution_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs)



add_library(
            avcodec-57
            SHARED
            IMPORTED)

set_target_properties( avcodec-57
                       PROPERTIES IMPORTED_LOCATION
                       ${distribution_DIR}/${ANDROID_ABI}/libavcodec-57.so )

add_library(
            avfilter-6
            SHARED
            IMPORTED)

set_target_properties( avfilter-6
                       PROPERTIES IMPORTED_LOCATION
                       ${distribution_DIR}/${ANDROID_ABI}/libavfilter-6.so )

add_library(
            avformat-57
            SHARED
            IMPORTED)

set_target_properties( avformat-57
                       PROPERTIES IMPORTED_LOCATION
                       ${distribution_DIR}/${ANDROID_ABI}/libavformat-57.so )

add_library(
            avutil-55
            SHARED
            IMPORTED)

set_target_properties( avutil-55
                       PROPERTIES IMPORTED_LOCATION
                       ${distribution_DIR}/${ANDROID_ABI}/libavutil-55.so )

add_library(
            swresample-2
            SHARED
            IMPORTED)
set_target_properties( swresample-2
                       PROPERTIES IMPORTED_LOCATION
                       ${distribution_DIR}/${ANDROID_ABI}/libswresample-2.so )

add_library(
            swscale-4
            SHARED
            IMPORTED)

set_target_properties( swscale-4
                       PROPERTIES IMPORTED_LOCATION
                       ${distribution_DIR}/${ANDROID_ABI}/libswscale-4.so )

add_library( avdevice-57
             SHARED
             IMPORTED)

set_target_properties( avdevice-57
                       PROPERTIES IMPORTED_LOCATION
                       ${distribution_DIR}/${ANDROID_ABI}/libavdevice-57.so )

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    message(STATUS "optional:-std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX)


find_library(
              log-lib
              log
               )

add_library(
             native-lib
             SHARED
             src/main/cpp/native-lib.cpp src/main/cpp/WlListener.cpp )

include_directories(src/main/jniLibs/include/)

target_link_libraries( native-lib OpenSLES android avcodec-57 avfilter-6 avformat-57 avutil-55 swresample-2 swscale-4 avdevice-57
                                              ${log-lib} )


