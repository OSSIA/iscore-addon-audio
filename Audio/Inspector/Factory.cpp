#include "Factory.hpp"
#include <Audio/SoundProcess/SoundProcessModel.hpp>
#include <Audio/Inspector/AudioInspector.hpp>
namespace Audio
{

InspectorFactory::InspectorFactory()
{

}

InspectorFactory::~InspectorFactory()
{

}

Process::InspectorWidgetDelegate* InspectorFactory::make(
        const Process::ProcessModel& process,
        const iscore::DocumentContext& doc,
        QWidget* parent) const
{
    return new InspectorWidget{
        static_cast<const Audio::Sound::ProcessModel&>(process),
                doc,
                parent};
}

bool InspectorFactory::matches(const Process::ProcessModel& process) const
{
    return dynamic_cast<const Audio::Sound::ProcessModel*>(&process);
}

}
