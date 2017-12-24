cmake_minimum_required(VERSION 3.4.1)

include_directories(./libs/jpeg)
link_directories(./libs/${ANDROID_ABI})

find_library(log-lib
             log)
find_library(android-lib
             android)
find_library(bitmap-lib
             jnigraphics)


add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp )

target_link_libraries( native-lib
                       ${log-lib}
                       ${android-lib}
                       ${bitmap-lib}
                       jpeg )