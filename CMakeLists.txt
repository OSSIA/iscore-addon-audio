cmake_minimum_required(VERSION 3.1)
project(iscore_plugin_audio LANGUAGES CXX)

if(NOT TARGET iscore_plugin_ossia)
    return()
endif()
find_package(Sndfile)
find_package(Faust REQUIRED)
# find_package(RtAudio)

if(NOT SNDFILE_LIBRARY)
message("audio plugin requires libsndfile, portaudio")
return()
endif()

add_subdirectory(3rdparty/libaudiostream)
iscore_common_setup()

# Packages
find_package(Qt5 5.3 REQUIRED COMPONENTS Core Widgets Multimedia)


qt5_add_resources(QRCS "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Panel/resources/audiopanel.qrc")

file(GLOB_RECURSE HDRS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libaudiotool/src/libwatermark/*.h")
# Files & main target
set(HDRS ${HDRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/AudioApplicationPlugin.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/AudioDocumentPlugin.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/AudioComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/AudioGraphNode.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/AudioDependencyGraph.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/AudioDependencyGraph.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Context.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Clock/AudioClock.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/EventComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/StateComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/TimeNodeComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ScenarioComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/LoopComponent.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Audio/SoundComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Audio/EffectComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Audio/SendComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Audio/ReturnComponent.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Utility.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TGroupAudioStream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/GroupAudioStream.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/GroupAudioStream.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/ExecutorAudioEffect.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TBufferManager.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TChannelAudioStream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TFixedLoopAudioStream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TGroupAudioMixer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TGroupAudioStream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TGroupRenderer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TPlayerAudioStream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TReturnAudioStream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TSendAudioStream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TSinusAudioStream.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioArray.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/ChangeAudioFile.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/ChangeSend.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/CreateSoundBox.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/AudioCommandFactory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/InsertEffect.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/UpdateMix.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/EditFaustEffect.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/Factory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/AudioInspector.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/EffectInspector.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/ReturnInspector.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/MixInspector.hpp"

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
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/Drop/SoundDrop.hpp"


    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/EffectProcessFactory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/EffectProcessMetadata.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/EffectProcessModel.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/EffectModel.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/EffectFactory.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/FaustEffectModel.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/FaustEffectFactory.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/MissingEffectModel.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LocalTree/LocalTreeEffectProcessComponent.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/MixProcess/Mix.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/MixProcess/MixProcessFactory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/MixProcess/MixProcessMetadata.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/MixProcess/MixProcessModel.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/InputProcess/InputProcessFactory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/InputProcess/InputProcessMetadata.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/InputProcess/InputProcessModel.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/GenProcess/GenProcessFactory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/GenProcess/GenProcessMetadata.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/GenProcess/GenProcessModel.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SendProcess/SendProcessFactory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SendProcess/SendProcessMetadata.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SendProcess/SendProcessModel.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/ReturnProcess/ReturnProcessFactory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/ReturnProcess/ReturnProcessMetadata.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/ReturnProcess/ReturnProcessModel.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/MediaFileHandle.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Panel/Track.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Panel/TrackListPanel.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Panel/TrackListPanelFactory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Panel/TrackModel.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/iscore_plugin_audio.hpp"
)

set(SRCS

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/EffectModelSerialization.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/FaustEffectModelSerialization.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/MissingEffectModelSerialization.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessLayerSerialization.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessModelSerialization.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/EffectProcessModelSerialization.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/MixProcess/MixProcessModelSerialization.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/InputProcess/InputProcessModelSerialization.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/GenProcess/GenProcessModelSerialization.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SendProcess/SendProcessModelSerialization.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/ReturnProcess/ReturnProcessModelSerialization.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/AudioApplicationPlugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/AudioDocumentPlugin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/AudioComponent.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Clock/AudioClock.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ConstraintComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/EventComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/StateComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/TimeNodeComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ProcessComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ScenarioComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/LoopComponent.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Audio/SoundComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Audio/EffectComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Audio/SendComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Audio/ReturnComponent.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Utility.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TGroupAudioStream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TBufferManager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TChannelAudioStream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TFixedLoopAudioStream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TGroupAudioMixer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TGroupAudioStream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TGroupRenderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TPlayerAudioStream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TReturnAudioStream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TSendAudioStream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TSinusAudioStream.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/ChangeAudioFile.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/ChangeSend.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/AudioCommandFactory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/InsertEffect.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/UpdateMix.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/EditFaustEffect.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/Factory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/AudioInspector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/EffectInspector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/ReturnInspector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/MixInspector.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsPresenter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsView.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessLayer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessPresenter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/SoundProcessView.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SoundProcess/Drop/SoundDrop.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/EffectProcessModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/EffectModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/EffectFactory.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/FaustEffectModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/FaustEffectFactory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/MissingEffectModel.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LocalTree/LocalTreeEffectProcessComponent.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/MixProcess/MixProcessModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/InputProcess/InputProcessModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/GenProcess/GenProcessModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SendProcess/SendProcessModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/ReturnProcess/ReturnProcessModel.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/MediaFileHandle.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Panel/Track.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Panel/TrackListPanel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Panel/TrackListPanelFactory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Panel/TrackModel.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/iscore_plugin_audio.cpp"
)

# LV2 support
find_package(Lilv)
if(TARGET Lilv)
    set(LV2_SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LV2/LV2EffectModel.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LV2/LV2EffectFactory.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LV2/LV2EffectModelSerialization.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LV2/LV2EffectModel.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LV2/LV2EffectFactory.cpp"
    )
endif()
add_library(${PROJECT_NAME} ${SRCS} ${HDRS} ${QRCS} ${LV2_SRCS})
set_property(TARGET ${PROJECT_NAME}  PROPERTY COTIRE_ADD_UNITY_BUILD FALSE)


iscore_generate_command_list_file(${PROJECT_NAME} "${HDRS}")
target_link_libraries(${PROJECT_NAME} PUBLIC
                     Qt5::Core Qt5::Widgets Qt5::Multimedia
                     iscore_lib_base iscore_plugin_ossia
                     ${SNDFILE_LIBRARIES}
                     libaudiostream)

target_include_directories(${PROJECT_NAME} PRIVATE
                     ${SNDFILE_INCLUDE_DIR})

if(TARGET Lilv)
    target_link_libraries(${PROJECT_NAME} PRIVATE Lilv)
endif()

target_link_libraries(${PROJECT_NAME}  PUBLIC ${FAUST_LIBRARIES})
target_include_directories(${PROJECT_NAME}  PRIVATE ${FAUST_INCLUDE_DIR})
setup_iscore_plugin(${PROJECT_NAME})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-error=return-type-c-linkage")
endif()

#add_subdirectory(Audio/CustomEngine/Faust)
#add_subdirectory(tests)
