#include "ProcessComponent.hpp"
#include <Process/Process.hpp>

Audio::AudioStreamEngine::ProcessComponent::ProcessComponent(
        Process::ProcessModel& process,
        DocumentPlugin& doc,
        const Id<iscore::Component>& id,
        const QString& name,
        QObject* parent):
    Scenario::GenericProcessComponent<DocumentPlugin>{process, doc, id, name, parent}
{
}

Audio::AudioStreamEngine::ProcessComponent::~ProcessComponent()
{

}

Audio::AudioStreamEngine::ProcessComponentFactory::~ProcessComponentFactory()
{

}
