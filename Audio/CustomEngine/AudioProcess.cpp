#include "AudioProcess.hpp"
#include <Audio/CustomEngine/AudioProcessModel.hpp>
#include <Audio/AudioDocumentPlugin.hpp>
#include <Editor/TimeConstraint.h>

namespace RecreateOnPlay
{
namespace Audio
{
Executor::Executor(
        AudioBlock& block,
        AudioEngine& conf):
    m_conf{conf},
    m_block{block},
    m_start{OSSIA::State::create()},
    m_end{OSSIA::State::create()}
{

}

std::shared_ptr<OSSIA::StateElement> Executor::state()
{
    double t = parent->getPosition();
    if(t == 0)
        m_block.start();
    if(t >= 0.99)
        m_block.stop();

    return {};
}

std::shared_ptr<OSSIA::StateElement> Executor::offset(const OSSIA::TimeValue & t)
{
    ISCORE_TODO;
    return OSSIA::State::create();
}

/// Component
Component::Component(
        RecreateOnPlay::ConstraintElement& parentConstraint,
        ::Audio::SoundProcess::ProcessModel& element,
        AudioBlock& block,
        const Context& ctx,
        const Id<iscore::Component>& id,
        QObject* parent):
    ProcessComponent{parentConstraint, element, id, "AudioComponent", parent}
{
    m_ossia_process = std::make_shared<Executor>(
                block,
                ctx.doc.plugin< ::Audio::AudioDocumentPlugin>().engine());
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
    auto& audio_proc = static_cast< ::Audio::SoundProcess::ProcessModel&>(proc);
    if(auto b = audio_proc.block())
    {
        return new Component{
            cst,
                    audio_proc,
                    *b,
                    ctx, id, parent};
    }

    return nullptr;
}

const ComponentFactory::ConcreteFactoryKey&
ComponentFactory::concreteFactoryKey() const
{
    static ComponentFactory::ConcreteFactoryKey k("9af86804-e9a5-4e6f-b765-2de26ebe745f");
    return k;
}

bool ComponentFactory::matches(
        ::Process::ProcessModel& proc,
        const DocumentPlugin&,
        const iscore::DocumentContext&) const
{
    return dynamic_cast< ::Audio::SoundProcess::ProcessModel*>(&proc);
}
}
}

