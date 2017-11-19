#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>

class QListWidget;
class QPushButton;

namespace Audio
{
namespace Effect
{
class InspectorWidget final :
        public Process::InspectorWidgetDelegate_T<ProcessModel>
{
    public:
        explicit InspectorWidget(
                const ProcessModel& object,
                const score::DocumentContext& doc,
                QWidget* parent);

    private:
        void recreate();
        QListWidget* m_list{};
        QPushButton* m_add{};
        CommandDispatcher<> m_dispatcher;
};
}
}

