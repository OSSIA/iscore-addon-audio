cmake_minimum_required(VERSION 3.1)
project(iscore_plugin_audio LANGUAGES CXX)

if(NOT TARGET iscore_plugin_ossia)
    return()
endif()
find_package(Sndfile)
find_package(RtAudio)

if(NOT SNDFILE_LIBRARY OR NOT RtAudio_LIBRARY)
message("audio plugin requires libsndfile, portaudio")
return()
endif()
# add_subdirectory(3rdparty/llvm)
set(FAUST_NO_EXECUTABLE True)
# add_subdirectory(3rdparty/faudiostream-code)
add_subdirectory(3rdparty/libaudiostream)
set(CMAKE_AUTOMOC OFF)
include_directories(3rdparty/libaudiotool/src/)

set(CMAKE_AUTOMOC ON)
iscore_common_setup()

# Packages
find_package(Qt5 5.3 REQUIRED COMPONENTS Core Widgets Multimedia)


file(GLOB_RECURSE HDRS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libaudiotool/src/libwatermark/*.h")
# Files & main target
set(HDRS ${HDRS}
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioFactory.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioProcess.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioProcessMetadata.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioProcessModel.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioEngine.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioBlock.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/WavBlock.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/AudioInspector.hpp"

"${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/Factory.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/Model.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/Presenter.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/View.hpp"

"${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioDocumentPlugin.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/iscore_plugin_audio.hpp"
)

set(SRCS
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioFactory.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioProcess.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioProcessModel.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioEngine.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioBlock.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/WavBlock.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/AudioInspector.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/Factory.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/Model.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/Presenter.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/View.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/iscore_plugin_audio.cpp"
)

add_library(${PROJECT_NAME} ${SRCS} ${HDRS})
iscore_generate_command_list_file(${PROJECT_NAME} "${HDRS}")
target_link_libraries(${PROJECT_NAME} PUBLIC
                     Qt5::Core Qt5::Widgets Qt5::Multimedia
                     iscore_lib_base iscore_plugin_ossia iscore_lib_dummyprocess
                                         ${RtAudio_LIBRARY} ${SNDFILE_LIBRARIES})

setup_iscore_plugin(${PROJECT_NAME})

add_subdirectory(Faust)
