cmake_minimum_required(VERSION 3.1)
project(iscore_component_audio_faust LANGUAGES CXX)

find_library(FAUST_LIBRARY faust)
if(NOT FAUST_LIBRARY)
message("FaUST not found; won't build FaUST component")
return()
endif()

message("${FAUST_LIBRARY}")

iscore_common_setup()
set(DEPS
    iscore_addon_audio
    ${FAUST_LIBRARY})


set(HDRS
"${CMAKE_CURRENT_SOURCE_DIR}/FaustAudioBlock.hpp"
)
set(SRCS
"${CMAKE_CURRENT_SOURCE_DIR}/FaustAudioBlock.cpp"
)

iscore_add_component(
    "${PROJECT_NAME}"
    "${SRCS}" "${HDRS}"
    "${DEPS}"
    1
    "784bbbf5-bab9-4451-aa3f-26792fe0f0cf")
