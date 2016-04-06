#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
#include <Audio/SoundProcess/SoundProcessMetadata.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>
class QLineEdit;
namespace iscore {
class Document;
}  // namespace iscore

namespace Audio
{
namespace SoundProcess
{
class ProcessModel;
}
class InspectorWidget final :
        public Process::InspectorWidgetDelegate_T<SoundProcess::ProcessModel>
{
    public:
        explicit InspectorWidget(
                const SoundProcess::ProcessModel& object,
                const iscore::DocumentContext& doc,
                QWidget* parent);

    private:
        QLineEdit* m_edit{};
        CommandDispatcher<> m_dispatcher;
};

}
