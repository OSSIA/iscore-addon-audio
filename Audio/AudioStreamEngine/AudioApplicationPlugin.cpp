#include "AudioApplicationPlugin.hpp"
#include "AudioDocumentPlugin.hpp"
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <Scenario/Application/ScenarioApplicationPlugin.hpp>
namespace Audio
{
namespace AudioStreamEngine
{

ApplicationPlugin::ApplicationPlugin(const iscore::ApplicationContext& app):
    iscore::GUIApplicationContextPlugin{app, "AudioApplicationPlugin", nullptr}
{

}

void ApplicationPlugin::on_newDocument(iscore::Document* doc)
{
    auto plug = new DocumentPlugin{*doc, &doc->model()};
    doc->model().addPluginModel(plug);

    auto& ctrl = doc->context().app.components.applicationPlugin<Scenario::ScenarioApplicationPlugin>();
    auto acts = ctrl.actions();
    for(const auto& act : acts)
    {
        if(act->objectName() == "Play")
        {
            connect(act, &QAction::toggled,
                    plug, [=] (bool b)
            { plug->play(); });
        }
        else if(act->objectName() == "Stop")
        {
            connect(act, &QAction::triggered,
                    plug, [=] (bool b) { plug->stop(); });
        }
    }
}

}
}
