#pragma once
#include <OSSIA/Executor/ProcessElement.hpp>
#include <OSSIA/ProcessModel/TimeProcessWithConstraint.hpp>
#include <Editor/TimeValue.h>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Audio/AudioBlock.hpp>
#include <Audio/AudioEngine.hpp>
#include <State/Value.hpp>
class DeviceDocumentPlugin;
class DeviceList;

namespace Audio
{
class ProcessModel;
}

namespace RecreateOnPlay
{
namespace Audio
{
class Executor final : public TimeProcessWithConstraint
{
    public:
        Executor(
                AudioBlock& block,
                AudioEngine& conf);

        std::shared_ptr<OSSIA::StateElement> state(
                const OSSIA::TimeValue& t,
                const OSSIA::TimeValue&) override;

        auto& block() const
        { return m_block; }

        auto& engine() const
        { return m_conf; }

    private:
        AudioEngine& m_conf;
        AudioBlock& m_block;

        std::shared_ptr<OSSIA::State> m_start;
        std::shared_ptr<OSSIA::State> m_end;
};


class Component final : public ProcessComponent
{
    public:
        Component(
                ConstraintElement& parentConstraint,
                ::Audio::ProcessModel& element,
                AudioBlock& block,
                const Context& ctx,
                const Id<iscore::Component>& id,
                QObject* parent);
        ~Component();

    private:
        void recreate();

        const Key& key() const override;
};


class ComponentFactory final :
        public ProcessComponentFactory
{
        ISCORE_COMPONENT_FACTORY(RecreateOnPlay::ProcessComponentFactory, RecreateOnPlay::Audio::ComponentFactory)
    public:
        virtual ~ComponentFactory();
        ProcessComponent* make(
                ConstraintElement& cst,
                ::Process::ProcessModel& proc,
                const Context& ctx,
                const Id<iscore::Component>& id,
                QObject* parent) const override;

        const ConcreteFactoryKey& concreteFactoryKey() const override;

        bool matches(
                ::Process::ProcessModel&,
                const DocumentPlugin&,
                const iscore::DocumentContext &) const override;
};
}
}
