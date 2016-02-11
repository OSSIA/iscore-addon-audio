#include "AudioProcess.hpp"
#include <Audio/AudioProcessModel.hpp>

namespace RecreateOnPlay
{
namespace Audio
{
Process::Process(AudioEngine& conf):
    m_conf{conf},
    m_start{OSSIA::State::create()},
    m_end{OSSIA::State::create()}
{

}


void Process::setAudioBlock(std::unique_ptr<AudioBlock>&& val)
{
    m_block = std::move(val);
}

std::shared_ptr<OSSIA::StateElement> Process::state(const OSSIA::TimeValue& t, const OSSIA::TimeValue&)
{
    if(double(t) == 0)
        if(m_block)
            m_block->start();
    if(double(t) >= 0.99)
        if(m_block)
            m_block->stop();

    return {};
}

const std::shared_ptr<OSSIA::State>&Process::getStartState() const
{
    return m_start;
}

const std::shared_ptr<OSSIA::State>&Process::getEndState() const
{
    return m_end;
}


/// Component
Component::Component(
        RecreateOnPlay::ConstraintElement& parentConstraint,
        ::Audio::ProcessModel& element,
        const Context& ctx,
        const Id<iscore::Component>& id,
        QObject* parent):
    ProcessComponent{parentConstraint, element, id, "AudioComponent", parent}
{
}

Component::~Component()
{
}

const iscore::Component::Key& Component::key() const
{
    static iscore::Component::Key k("AudioElement");
    return k;
}


/// Component Factory
ComponentFactory::~ComponentFactory()
{

}

ProcessComponent* ComponentFactory::make(
        ConstraintElement& cst,
        ::Process::ProcessModel& proc,
        const Context& ctx,
        const Id<iscore::Component>& id,
        QObject* parent) const
{
    return new Component{
                cst,
                static_cast< ::Audio::ProcessModel&>(proc),
                ctx, id, parent};

}

const ComponentFactory::ConcreteFactoryKey&
ComponentFactory::concreteFactoryKey() const
{
    static ComponentFactory::ConcreteFactoryKey k("60e3b412-559d-4385-9a58-bdcd19bb9fa7");
    return k;
}

bool ComponentFactory::matches(
        ::Process::ProcessModel& proc,
        const DocumentPlugin&,
        const iscore::DocumentContext&) const
{
    return dynamic_cast< ::Audio::ProcessModel*>(&proc);
}
}
}

