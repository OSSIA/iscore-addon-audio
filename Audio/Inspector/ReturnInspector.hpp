#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <Audio/ReturnProcess/ReturnProcessModel.hpp>

namespace Audio
{
namespace Return
{
class ProcessModel;
class InspectorWidget final :
        public Process::InspectorWidgetDelegate_T<ProcessModel>
{
    public:
        explicit InspectorWidget(
                const ProcessModel& object,
                const score::DocumentContext& doc,
                QWidget* parent);

    private:
        CommandDispatcher<> m_dispatcher;
};
}
}
