cmake_minimum_required(VERSION 3.10.2)

include_directories(${CMAKE_SOURCE_DIR}/cpp/include)

file(GLOB_RECURSE allCpp *.cpp *.h)

# 设置库文件的路径别名
set(JNILIB_PATH ${CMAKE_SOURCE_DIR}/jniLibs/${CMAKE_ANDROID_ARCH_ABI})

# 导入库文件
add_library(avcodec SHARED IMPORTED)
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION ${JNILIB_PATH}/libavcodec.so)

add_library(avfilter SHARED IMPORTED)
set_target_properties(avfilter PROPERTIES IMPORTED_LOCATION ${JNILIB_PATH}/libavfilter.so)

add_library(avformat SHARED IMPORTED)
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION ${JNILIB_PATH}/libavformat.so)

add_library(avutil SHARED IMPORTED)
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION ${JNILIB_PATH}/libavutil.so)

add_library(swresample SHARED IMPORTED)
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION ${JNILIB_PATH}/libswresample.so)

add_library(swscale SHARED IMPORTED)
set_target_properties(swscale PROPERTIES IMPORTED_LOCATION ${JNILIB_PATH}/libswscale.so)

add_library(
        native-lib
        SHARED
        ${allCpp}
)

find_library(
        log-lib
        log
)

target_link_libraries(
        native-lib
        ${log-lib}
        z
        # 视频渲染需要用到ANativeWindow
        android
        # 播放音频
        OpenSLES
        avcodec
        avformat
        avfilter
        avutil
        swresample
        swscale
)