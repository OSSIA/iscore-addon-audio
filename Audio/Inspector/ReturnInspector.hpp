#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
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
                const iscore::DocumentContext& doc,
                QWidget* parent);

    private:
        CommandDispatcher<> m_dispatcher;
};
}
}
