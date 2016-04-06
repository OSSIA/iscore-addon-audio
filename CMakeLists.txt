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
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/AudioApplicationPlugin.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/AudioDocumentPlugin.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/EventComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/StateComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/TimeNodeComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ScenarioComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ScenarioComponentFactory.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioArray.hpp"

#    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/CustomEngine/AudioBlock.hpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/CustomEngine/AudioEngine.hpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/CustomEngine/AudioProcess.hpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/CustomEngine/WavBlock.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/ChangeAudioFile.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/AudioCommandFactory.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/Factory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/AudioInspector.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsFactory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsModel.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsPresenter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsView.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessFactory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessLayer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessLayerPanelProxy.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessMetadata.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessModel.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessPresenter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessView.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/MediaFileHandle.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/iscore_plugin_audio.hpp"
)

set(SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessLayerSerialization.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessModelSerialization.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/AudioApplicationPlugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/AudioDocumentPlugin.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ConstraintComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/EventComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/StateComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/TimeNodeComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ProcessComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ScenarioComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ScenarioComponentFactory.cpp"

#    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/CustomEngine/AudioBlock.cpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/CustomEngine/AudioEngine.cpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/CustomEngine/AudioProcess.cpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/CustomEngine/WavBlock.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/ChangeAudioFile.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/AudioCommandFactory.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/Factory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/AudioInspector.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsFactory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsPresenter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsView.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessFactory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessLayer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessLayerPanelProxy.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessPresenter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessView.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/MediaFileHandle.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/iscore_plugin_audio.cpp"
)

add_library(${PROJECT_NAME} ${SRCS} ${HDRS})
iscore_generate_command_list_file(${PROJECT_NAME} "${HDRS}")
target_link_libraries(${PROJECT_NAME} PUBLIC
                     Qt5::Core Qt5::Widgets Qt5::Multimedia
                     iscore_lib_base iscore_plugin_ossia iscore_lib_dummyprocess
                     ${SNDFILE_LIBRARIES})

setup_iscore_plugin(${PROJECT_NAME})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-error=return-type-c-linkage")
endif()

#add_subdirectory(Audio/CustomEngine/Faust)
