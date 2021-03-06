#include "ProcessComponent.hpp"
#include <Process/Process.hpp>
#include <QStringBuilder>
namespace Audio
{
namespace AudioStreamEngine
{

ProcessComponent::ProcessComponent(
        Process::ProcessModel& process,
        DocumentPlugin& doc,
        const Id<score::Component>& id,
        const QString& name,
        QObject* parent):
    Process::ProcessComponent<Component>{process, doc, id, name, parent}
{
}

ProcessComponent::~ProcessComponent()
{

}

QString ProcessComponent::prettyName() const
{
    return process().metadata().getName() % " " % QString::number(process().id_val());
}

ProcessComponentFactory::~ProcessComponentFactory()
{

}

}
}
