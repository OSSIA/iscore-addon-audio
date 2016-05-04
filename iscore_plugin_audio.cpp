#include "iscore_plugin_audio.hpp"
#include <Audio/SoundProcess/SoundProcessFactory.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <core/document/Document.hpp>
#include <iscore/plugins/application/GUIApplicationContextPlugin.hpp>
#include <Scenario/Application/ScenarioApplicationPlugin.hpp>
#include <iscore/plugins/customfactory/FactoryFamily.hpp>
#include <core/document/DocumentModel.hpp>
#include <iscore/plugins/customfactory/FactorySetup.hpp>
#include <QAction>
#include <Audio/Inspector/Factory.hpp>
#include <Audio/SoundProcess/SoundProcessFactory.hpp>
#include <Audio/EffectProcess/EffectProcessFactory.hpp>
#include <Audio/MixProcess/MixProcessFactory.hpp>
#include <Audio/SendProcess/SendProcessFactory.hpp>
#include <Audio/ReturnProcess/ReturnProcessFactory.hpp>
#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>
#include <Audio/Settings/Card/CardSettingsFactory.hpp>

#include <Audio/AudioStreamEngine/Scenario/ScenarioComponentFactory.hpp>
#include <Audio/AudioStreamEngine/Scenario/LoopComponentFactory.hpp>
#include <Audio/AudioStreamEngine/Audio/EffectComponentFactory.hpp>
#include <Audio/AudioStreamEngine/Audio/SoundComponentFactory.hpp>
#include <Audio/AudioStreamEngine/Audio/MixComponentFactory.hpp>
#include <Audio/AudioStreamEngine/Audio/SendComponentFactory.hpp>
#include <Audio/AudioStreamEngine/Audio/ReturnComponentFactory.hpp>
#include <iscore_plugin_audio_commands_files.hpp>

std::pair<const CommandParentFactoryKey, CommandGeneratorMap> iscore_plugin_audio::make_commands()
{
    using namespace Audio::Commands;
    std::pair<const CommandParentFactoryKey, CommandGeneratorMap> cmds{Audio::CommandFactoryName(), CommandGeneratorMap{}};

    using Types = TypeList<
#include <iscore_plugin_audio_commands.hpp>
      >;
    for_each_type<Types>(iscore::commands::FactoryInserter{cmds.second});

    return cmds;
}

std::vector<std::unique_ptr<iscore::FactoryInterfaceBase>> iscore_plugin_audio::factories(
        const iscore::ApplicationContext& ctx,
        const iscore::AbstractFactoryKey& key) const
{
    return instantiate_factories<
            iscore::ApplicationContext,
    TL<
        FW<Process::ProcessFactory,
            Audio::Sound::ProcessFactory,
            Audio::Effect::ProcessFactory,
            Audio::Mix::ProcessFactory,
            Audio::Send::ProcessFactory,
            Audio::Return::ProcessFactory
            >,
        FW<Audio::AudioStreamEngine::ProcessComponentFactory,
            Audio::AudioStreamEngine::EffectComponentFactory,
            Audio::AudioStreamEngine::SoundComponentFactory,
            Audio::AudioStreamEngine::MixComponentFactory,
            Audio::AudioStreamEngine::SendComponentFactory,
            Audio::AudioStreamEngine::ReturnComponentFactory,
            Audio::AudioStreamEngine::ScenarioComponentFactory,
            Audio::AudioStreamEngine::LoopComponentFactory
            >,
            /*
        FW<RecreateOnPlay::ProcessComponentFactory,
            RecreateOnPlay::Audio::ComponentFactory>,*/
        FW<Process::InspectorWidgetDelegateFactory,
            Audio::Sound::InspectorFactory,
            Audio::Mix::InspectorFactory,
            Audio::Return::InspectorFactory
            >,
        FW<iscore::SettingsDelegateFactory,
            Audio::Settings::Factory>
    >>(ctx, key);
}

iscore_plugin_audio::iscore_plugin_audio()
{

}

iscore_plugin_audio::~iscore_plugin_audio()
{

}

iscore::GUIApplicationContextPlugin*iscore_plugin_audio::make_applicationPlugin(const iscore::ApplicationContext& app)
{
    return new Audio::AudioStreamEngine::ApplicationPlugin{app};
}

std::vector<std::unique_ptr<iscore::FactoryListInterface> > iscore_plugin_audio::factoryFamilies()
{
    return make_ptr_vector<iscore::FactoryListInterface,
            Audio::AudioStreamEngine::ProcessComponentFactoryList>();
}

iscore::Version iscore_plugin_audio::version() const
{
    return iscore::Version{1};
}

UuidKey<iscore::Plugin> iscore_plugin_audio::key() const
{
    return "f07abe79-1b83-4abd-b002-958c878755c1";
}
