cmake_minimum_required(VERSION 3.4.1)

set(srcs)
list(APPEND srcs
        dvb_jni.cpp
        DvbDevice.cpp
        )

add_library(dvb SHARED ${srcs})

target_link_libraries(dvb
        log
        )