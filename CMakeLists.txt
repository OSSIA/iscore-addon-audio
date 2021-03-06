cmake_minimum_required(VERSION 3.1)
project(score_plugin_audio LANGUAGES CXX)

# Dependencies

if(NOT TARGET score_plugin_engine)
    return()
endif()

set(LIBAUDIOSTREAM_FRAMEWORK OFF)
set(LIBAUDIOSTREAM_SHARED OFF)
set(LIBAUDIOSTREAM_CONTAINED ON)
if(APPLE)
set(OPENSSL_ROOT_DIR /usr/local/Cellar/openssl/1.0.2l)
endif()
add_subdirectory(3rdparty/libaudiostream)
#set(SCORE_AUDIO_PLUGINS 1)
if(SCORE_AUDIO_PLUGINS)
  find_package(Lilv)
  find_package(Faust)
endif()

score_common_setup()

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
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/SyncComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ScenarioComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/LoopComponent.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Audio/SoundComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Audio/InputComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Audio/EffectComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Audio/SendComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Audio/ReturnComponent.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Utility.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TGroupAudioStream.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/ExecutorAudioEffect.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TBufferManager.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TChannelAudioStream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TFixedLoopAudioStream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TGroupAudioMixer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TGroupAudioStream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TSimpleBufferAudioStream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TGroupRenderer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TPlayerAudioStream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TReturnAudioStream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TSendAudioStream.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TSinusAudioStream.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/ChangeSend.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/AudioCommandFactory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/InsertEffect.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/UpdateMix.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/CreateEffectAutomation.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/Factory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/EffectInspector.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/ReturnInspector.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/MixInspector.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/PressableWidget.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsFactory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsModel.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsPresenter.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsView.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/EffectProcessFactory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/EffectProcessMetadata.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/EffectProcessModel.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/EffectParameters.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/EffectModel.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/EffectComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/EffectFactory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/Widgets/EffectSlider.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/Widgets/EffectWidget.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/Widgets/EffectListWidget.hpp"


    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/Missing/MissingEffectModel.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LocalTree/LocalTreeEffectProcessComponent.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LocalTree/LocalTreeEffectComponent.hpp"

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

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Panel/Track.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Panel/TrackListPanel.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Panel/TrackListPanelFactory.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Panel/TrackModel.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/iscore_plugin_audio.hpp"
)

set(SRCS

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/EffectModelSerialization.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/Missing/MissingEffectModelSerialization.cpp"

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
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/SyncComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ProcessComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/ScenarioComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Scenario/LoopComponent.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Audio/SoundComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Audio/InputComponent.cpp"
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
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TSimpleBufferAudioStream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TGroupAudioStream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TGroupRenderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TPlayerAudioStream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TReturnAudioStream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TSendAudioStream.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/AudioStreamEngine/Streams/TSinusAudioStream.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/ChangeSend.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/AudioCommandFactory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/InsertEffect.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/UpdateMix.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/EffectInspector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/ReturnInspector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Inspector/MixInspector.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsPresenter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Settings/Card/CardSettingsView.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/EffectProcessModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/EffectModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/EffectFactory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/Widgets/EffectSlider.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/Widgets/EffectWidget.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/Widgets/EffectListWidget.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/Missing/MissingEffectModel.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LocalTree/LocalTreeEffectProcessComponent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LocalTree/LocalTreeEffectComponent.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/MixProcess/MixProcessModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/InputProcess/InputProcessModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/GenProcess/GenProcessModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/SendProcess/SendProcessModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/ReturnProcess/ReturnProcessModel.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Panel/Track.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Panel/TrackListPanel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Panel/TrackListPanelFactory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Panel/TrackModel.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/iscore_plugin_audio.cpp"
)
# Faust support
if(${Faust_FOUND})
    set(FAUST_HDRS
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/EditFaustEffect.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/Faust/FaustEffectModel.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LocalTree/LocalTreeFaustEffectComponent.hpp"
        )
    set(FAUST_SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/Commands/EditFaustEffect.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/Faust/FaustEffectModelSerialization.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Effect/Faust/FaustEffectModel.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LocalTree/LocalTreeFaustEffectComponent.cpp"
        )
endif()

# LV2 support
if(TARGET Lilv)
    set(LV2_HDRS
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LV2/LV2EffectModel.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LV2/LV2EffectFactory.hpp"
        )
    set(LV2_SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LV2/LV2EffectModelSerialization.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LV2/LV2EffectModel.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/LV2/LV2EffectFactory.cpp"
    )
endif()

set(ENVELOPE_HDRS
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Envelope/EnvelopeEffectModel.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Envelope/EnvelopeEffectFactory.hpp"
    )
set(ENVELOPE_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Envelope/EnvelopeEffectModelSerialization.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Envelope/EnvelopeEffectModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Audio/EffectProcess/Envelope/EnvelopeEffectFactory.cpp"
)
add_library(${PROJECT_NAME}
    ${SRCS} ${HDRS} ${QRCS}
    ${LV2_SRCS} ${LV2_HDRS}
    ${ENVELOPE_SRCS} ${ENVELOPE_HDRS}
    ${FAUST_HDRS} ${FAUST_SRCS}

    )
set_property(TARGET ${PROJECT_NAME} PROPERTY COTIRE_ADD_UNITY_BUILD FALSE)
set_property(TARGET ${PROJECT_NAME} PROPERTY SCORE_TARGET_NO_LTO TRUE)
set_property(TARGET ${PROJECT_NAME} PROPERTY SCORE_TARGET_NO_SANITIZE TRUE)


score_generate_command_list_file(${PROJECT_NAME} "${HDRS};${LV2_HDRS};${FAUST_HDRS}")
target_link_libraries(${PROJECT_NAME} PUBLIC
                     Qt5::Core Qt5::Widgets Qt5::Multimedia
                     score_lib_base score_plugin_engine LibAudioStream
)
if(NOT ANDROID)
if(APPLE)
target_link_libraries(${PROJECT_NAME} PUBLIC -L/usr/local/lib)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC
            score_plugin_midi score_plugin_media)
endif()

if(${Faust_FOUND})
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAS_FAUST)
endif()

if(TARGET Lilv)
    target_link_libraries(${PROJECT_NAME} PRIVATE Lilv)
    target_include_directories(${PROJECT_NAME} PRIVATE 3rdparty/readerwriterqueue)
endif()

if(${Faust_FOUND})
  target_link_libraries(${PROJECT_NAME}  PUBLIC ${FAUST_LIBRARIES})
  target_include_directories(${PROJECT_NAME}  PRIVATE ${FAUST_INCLUDE_DIR})
endif()
setup_score_plugin(${PROJECT_NAME})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-error=return-type-c-linkage")
endif()


# Copy the faust .lib files so that they can be used for the effects.
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/3rdpary/faust-lib "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
# TODO install
#add_subdirectory(Audio/CustomEngine/Faust)
#add_subdirectory(tests)
