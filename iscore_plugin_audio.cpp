#include "iscore_plugin_audio.hpp"
#include <Audio/AudioFactory.hpp>
#include <Audio/AudioDocumentPlugin.hpp>
#include <core/document/Document.hpp>
#include <iscore/plugins/application/GUIApplicationContextPlugin.hpp>
#include <Scenario/Application/ScenarioApplicationPlugin.hpp>
#include <Audio/AudioProcess.hpp>
#include <iscore/plugins/customfactory/FactoryFamily.hpp>
#include <core/document/DocumentModel.hpp>
#include <iscore/plugins/customfactory/FactorySetup.hpp>
#include <QAction>
#include <Audio/Inspector/AudioInspector.hpp>

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
            Audio::ProcessFactory>,
        FW<RecreateOnPlay::ProcessComponentFactory,
             RecreateOnPlay::Audio::ComponentFactory>,
        FW<Process::InspectorWidgetDelegateFactory,
            Audio::InspectorFactory>
    >>(ctx, key);
}

iscore_plugin_audio::iscore_plugin_audio()
{

}

iscore_plugin_audio::~iscore_plugin_audio()
{

}
namespace Audio
{
class ApplicationPlugin : public QObject, public iscore::GUIApplicationContextPlugin
{
    public:
        ApplicationPlugin(const iscore::ApplicationContext& app):
            iscore::GUIApplicationContextPlugin{app, "AudioApplicationPlugin", nullptr}
        {

        }

        void on_newDocument(iscore::Document* doc) override
        {
            auto plug = new AudioDocumentPlugin{*doc, &doc->model()};
            doc->model().addPluginModel(plug);

            auto& ctrl = doc->context().app.components.applicationPlugin<Scenario::ScenarioApplicationPlugin>();
            auto acts = ctrl.actions();
            for(const auto& act : acts)
            {
                if(act->objectName() == "Play")
                {
                    connect(act, &QAction::toggled,
                            plug, [=] (bool b)
                    { plug->engine().play(); });
                }
                else if(act->objectName() == "Stop")
                {
                    connect(act, &QAction::triggered,
                            plug, [=] (bool b) { plug->engine().stop(); });
                }
            }
        }

};
}

iscore::GUIApplicationContextPlugin*iscore_plugin_audio::make_applicationPlugin(const iscore::ApplicationContext& app)
{
    return new Audio::ApplicationPlugin{app};
}

iscore::Version iscore_plugin_audio::version() const
{
    return iscore::Version{1};
}

UuidKey<iscore::Plugin> iscore_plugin_audio::key() const
{
    return "f07abe79-1b83-4abd-b002-958c878755c1";
}
