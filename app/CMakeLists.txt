# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
include_directories(src/main/jniLibs/includes)
set(WS_MSVC_NORETURN " ")


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

file(GLOB SRC_FILES
            src/main/cpp/*.cpp
            src/main/cpp/*.h
            src/main/cpp/extstdio/*.cpp
            src/main/cpp/extstdio/*.h)


add_library( # Sets the name of the library.
             diagRevealer

             # Sets the library as a shared library.
             SHARED


                ${SRC_FILES})


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.

                        diagRevealer
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

set(LIBS_DIR ${PROJECT_SOURCE_DIR}/src/main/jniLibs)

add_library(LIBWIRESHARK SHARED IMPORTED)
set_target_properties(LIBWIRESHARK PROPERTIES
IMPORTED_LOCATION ${LIBS_DIR}/${ANDROID_ABI}/libwireshark.so)

add_library(LIBWSUTIL SHARED IMPORTED)
set_target_properties(LIBWSUTIL PROPERTIES
IMPORTED_LOCATION ${LIBS_DIR}/${ANDROID_ABI}/libwsutil.so)

add_library(LIBWIRETAP SHARED IMPORTED)
set_target_properties(LIBWIRETAP PROPERTIES
IMPORTED_LOCATION ${LIBS_DIR}/${ANDROID_ABI}/libwiretap.so)

add_library(LIBGTHREAD SHARED IMPORTED)
set_target_properties(LIBGTHREAD PROPERTIES
IMPORTED_LOCATION ${LIBS_DIR}/${ANDROID_ABI}/libgthread-2.0.so)

add_library(LIBGOBJECT SHARED IMPORTED)
set_target_properties(LIBGOBJECT PROPERTIES
IMPORTED_LOCATION ${LIBS_DIR}/${ANDROID_ABI}/libgobject-2.0.so)

add_library(LIBGMODULE SHARED IMPORTED)
set_target_properties(LIBGMODULE PROPERTIES
IMPORTED_LOCATION ${LIBS_DIR}/${ANDROID_ABI}/libgmodule-2.0.so)

add_library(LIBGLIB SHARED IMPORTED)
set_target_properties(LIBGLIB PROPERTIES
IMPORTED_LOCATION ${LIBS_DIR}/${ANDROID_ABI}/libglib-2.0.so)


target_link_libraries(diagRevealer LIBWIRESHARK LIBWSUTIL LIBWIRETAP LIBGTHREAD LIBGOBJECT LIBGMODULE LIBGLIB)

