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
#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>
#include <Audio/Settings/Card/CardSettingsFactory.hpp>

std::pair<const CommandParentFactoryKey, CommandGeneratorMap> iscore_plugin_audio::make_commands()
{
    return {};
}

std::vector<std::unique_ptr<iscore::FactoryInterfaceBase>> iscore_plugin_audio::factories(
        const iscore::ApplicationContext& ctx,
        const iscore::AbstractFactoryKey& key) const
{
    return instantiate_factories<
            iscore::ApplicationContext,
    TL<
        FW<Process::ProcessFactory,
            Audio::SoundProcess::ProcessFactory>,/*
        FW<RecreateOnPlay::ProcessComponentFactory,
            RecreateOnPlay::Audio::ComponentFactory>,*/
        FW<Process::InspectorWidgetDelegateFactory,
            Audio::InspectorFactory>,
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

iscore::Version iscore_plugin_audio::version() const
{
    return iscore::Version{1};
}

UuidKey<iscore::Plugin> iscore_plugin_audio::key() const
{
    return "f07abe79-1b83-4abd-b002-958c878755c1";
}
