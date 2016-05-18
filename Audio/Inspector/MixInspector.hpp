#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <iscore/command/Dispatchers/OngoingCommandDispatcher.hpp>
#include <Audio/MixProcess/MixProcessModel.hpp>

class QTableWidget;

namespace Audio
{
namespace Mix
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
        void recreate();

        QTableWidget* m_table{};
        OngoingCommandDispatcher m_dispatcher;
};
}
}
