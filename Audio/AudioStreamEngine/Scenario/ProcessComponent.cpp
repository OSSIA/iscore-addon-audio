#include "ProcessComponent.hpp"
#include <Process/Process.hpp>

Audio::AudioStreamEngine::ProcessComponent::ProcessComponent(
        Process::ProcessModel& process,
        const Id<iscore::Component>& id,
        const QString& name,
        QObject* parent):
    Component{id, name, parent},
    m_process{process}
{
}

Audio::AudioStreamEngine::ProcessComponent::~ProcessComponent()
{

}

Audio::AudioStreamEngine::ProcessComponentFactory::~ProcessComponentFactory()
{

}
