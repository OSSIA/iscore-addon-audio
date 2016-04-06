#pragma once
#include <OSSIA/Executor/ProcessElement.hpp>
#include <Editor/TimeProcess.h>
#include <Editor/TimeValue.h>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Audio/CustomEngine/AudioBlock.hpp>
#include <Audio/CustomEngine/AudioEngine.hpp>
#include <State/Value.hpp>
class DeviceDocumentPlugin;
class DeviceList;

namespace Audio
{
namespace SoundProcess
{
class ProcessModel;
}
}
// TODO do it the other way round
namespace RecreateOnPlay
{
namespace Audio
{
class Executor final : public OSSIA::TimeProcess
{
    public:
        Executor(
                AudioBlock& block,
                AudioEngine& conf);

        std::shared_ptr<OSSIA::StateElement> state() override;
        std::shared_ptr<OSSIA::StateElement> offset(const OSSIA::TimeValue &) override;

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
                ::Audio::SoundProcess::ProcessModel& element,
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
