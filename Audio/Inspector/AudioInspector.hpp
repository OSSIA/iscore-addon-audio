#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>

class QLineEdit;

namespace Audio
{
namespace Sound
{
class InspectorWidget final :
        public Process::InspectorWidgetDelegate_T<Sound::ProcessModel>
{
    public:
        explicit InspectorWidget(
                const ProcessModel& object,
                const iscore::DocumentContext& doc,
                QWidget* parent);

    private:
        QLineEdit* m_edit{};
        CommandDispatcher<> m_dispatcher;
};
}
}
