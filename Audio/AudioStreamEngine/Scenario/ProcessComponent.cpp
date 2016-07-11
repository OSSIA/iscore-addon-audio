#include "ProcessComponent.hpp"
#include <Process/Process.hpp>

Audio::AudioStreamEngine::ProcessComponent::ProcessComponent(
        Process::ProcessModel& process,
        DocumentPlugin& doc,
        const Id<iscore::Component>& id,
        const QString& name,
        QObject* parent):
    Scenario::ProcessComponent<Component>{process, doc, id, name, parent}
{
}

Audio::AudioStreamEngine::ProcessComponent::~ProcessComponent()
{

}

Audio::AudioStreamEngine::ProcessComponentFactory::~ProcessComponentFactory()
{

}
