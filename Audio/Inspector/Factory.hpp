#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegateFactory.hpp>

namespace Audio
{
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
