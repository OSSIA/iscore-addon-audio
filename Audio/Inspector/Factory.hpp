#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegateFactory.hpp>

namespace Audio
{
namespace Sound
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

// MOVEME
namespace Mix
{
class InspectorFactory final :
        public Process::InspectorWidgetDelegateFactory
{
        ISCORE_CONCRETE_FACTORY_DECL("60efbf22-8374-4923-8a07-8caa4d242954")
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

// MOVEME
namespace Return
{
class InspectorFactory final :
        public Process::InspectorWidgetDelegateFactory
{
        ISCORE_CONCRETE_FACTORY_DECL("a5afd64d-750b-47c4-90ea-9788f47df9b8")
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
}
