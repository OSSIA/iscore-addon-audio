#include "iscore_plugin_audio.hpp"
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <Scenario/Application/ScenarioApplicationPlugin.hpp>
#include <QAction>
#include <Audio/Inspector/Factory.hpp>

#include <Audio/EffectProcess/EffectProcessFactory.hpp>
#include <Audio/MixProcess/MixProcessFactory.hpp>
#include <Audio/SendProcess/SendProcessFactory.hpp>
#include <Audio/ReturnProcess/ReturnProcessFactory.hpp>
#include <Audio/InputProcess/InputProcessFactory.hpp>

#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>
#include <Audio/Settings/Card/CardSettingsFactory.hpp>
#include <Audio/Panel/TrackListPanelFactory.hpp>

#include <Audio/EffectProcess/Effect/EffectFactory.hpp>
#include <Audio/EffectProcess/Effect/Faust/FaustEffectModel.hpp>

#include <Audio/EffectProcess/LocalTree/LocalTreeEffectProcessComponent.hpp>

#include <Audio/AudioStreamEngine/Scenario/ScenarioComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/LoopComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/EffectComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/SoundComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/SendComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/ReturnComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/InputComponent.hpp>

#include <Audio/AudioStreamEngine/Clock/AudioClock.hpp>

#include <score/plugins/application/GUIApplicationPlugin.hpp>
#include <score/plugins/customfactory/FactoryFamily.hpp>
#include <score/plugins/customfactory/FactorySetup.hpp>
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <score_plugin_audio_commands_files.hpp>

#if defined(LILV_SHARED) // TODO instead add a proper preprocessor macro that also works in static case
#include <Audio/EffectProcess/LV2/LV2EffectModel.hpp>
#endif
std::pair<const CommandGroupKey, CommandGeneratorMap> SCORE_plugin_audio::make_commands()
{
    using namespace Audio::Commands;
    std::pair<const CommandGroupKey, CommandGeneratorMap> cmds{Audio::CommandFactoryName(), CommandGeneratorMap{}};

    using Types = TypeList<
#include <score_plugin_audio_commands.hpp>
      >;
    for_each_type<Types>(score::commands::FactoryInserter{cmds.second});

    return cmds;
}

std::vector<std::unique_ptr<score::InterfaceBase>> SCORE_plugin_audio::factories(
        const score::ApplicationContext& ctx,
        const score::InterfaceKey& key) const
{
    return instantiate_factories<
            score::ApplicationContext,
        FW<Process::ProcessModelFactory,
            Audio::Effect::ProcessFactory,
            Audio::Mix::ProcessFactory,
            Audio::Send::ProcessFactory,
            Audio::Return::ProcessFactory,
            Audio::Input::ProcessFactory
            >,
        FW<Process::LayerFactory,
            Audio::Effect::LayerFactory,
            Audio::Mix::LayerFactory,
            Audio::Send::LayerFactory,
            Audio::Return::LayerFactory,
            Audio::Input::LayerFactory
            >,
        FW<Audio::AudioStreamEngine::ProcessComponentFactory,
            Audio::AudioStreamEngine::EffectProcessComponentFactory,
            Audio::AudioStreamEngine::SoundComponentFactory,
            Audio::AudioStreamEngine::SendComponentFactory,
            Audio::AudioStreamEngine::ReturnComponentFactory,
            Audio::AudioStreamEngine::InputComponentFactory,
            Audio::AudioStreamEngine::ScenarioComponentFactory,
            Audio::AudioStreamEngine::LoopComponentFactory
            >,
        FW<Process::InspectorWidgetDelegateFactory,
            Audio::Mix::InspectorFactory,
            Audio::Return::InspectorFactory,
            Audio::Effect::InspectorFactory
            >,
        FW<score::SettingsDelegateFactory,
            Audio::Settings::Factory>,/*
        FW<score::PanelDelegateFactory,
            Audio::Panel::TrackListPanelFactory>,*/
        FW<Audio::Effect::EffectFactory
#if defined(HAS_FAUST)
          , Audio::Effect::FaustEffectFactory
# endif
#if defined(LILV_SHARED)
            , Audio::Effect::LV2EffectFactory
#endif
            >,
        FW<Engine::LocalTree::ProcessComponentFactory,
            Audio::Effect::LocalTree::EffectProcessComponentFactory>,
        FW<Engine::Execution::ClockManagerFactory,
            Audio::AudioStreamEngine::AudioClockFactory>
    >(ctx, key);
}

SCORE_plugin_audio::SCORE_plugin_audio()
{

}

SCORE_plugin_audio::~SCORE_plugin_audio()
{

}

score::ApplicationPlugin*SCORE_plugin_audio::make_applicationPlugin(
    const score::ApplicationContext& app)
{
  return new Audio::AudioStreamEngine::ApplicationPlugin{app};
}
score::GUIApplicationPlugin*SCORE_plugin_audio::make_guiApplicationPlugin(
        const score::GUIApplicationContext& app)
{
    return new Audio::AudioStreamEngine::GUIApplicationPlugin{app};
}

std::vector<std::unique_ptr<score::InterfaceListBase> > SCORE_plugin_audio::factoryFamilies()
{
    return make_ptr_vector<score::InterfaceListBase,
            Audio::AudioStreamEngine::ProcessComponentFactoryList,
            Audio::Effect::EffectFactoryList>();
}
