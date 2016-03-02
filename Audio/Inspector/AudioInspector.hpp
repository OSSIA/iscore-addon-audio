#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegateFactory.hpp>
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
#include <Audio/AudioProcessMetadata.hpp>
#include <Audio/AudioProcessModel.hpp>
class QLineEdit;
namespace iscore {
class Document;
}  // namespace iscore

namespace Audio
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
        QLineEdit* m_edit{};
        CommandDispatcher<> m_dispatcher;
};

class InspectorFactory final :
        public Process::InspectorWidgetDelegateFactory
{
        ISCORE_CONCRETE_FACTORY_DECL("10360b4c-4d73-4274-8339-408644408cab")
    public:
        InspectorFactory();
        virtual ~InspectorFactory();

    private:
        Process::InspectorWidgetDelegate* make(
                const Process::ProcessModel&,
                const iscore::DocumentContext&,
                QWidget* parent) const override;
        bool matches(const Process::ProcessModel&) const override;
};

}
