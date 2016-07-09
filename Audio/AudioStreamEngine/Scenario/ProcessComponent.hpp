#pragma once
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <Process/Process.hpp>
#include <iscore/component/Component.hpp>
#include <iscore/component/ComponentFactory.hpp>
#include <iscore_plugin_audio_export.h>
#include <Scenario/Document/Components/ProcessComponent.hpp>
// TODO clean me up
namespace Audio
{
namespace AudioStreamEngine
{


class ISCORE_PLUGIN_AUDIO_EXPORT ProcessComponent :
        public Scenario::GenericProcessComponent<DocumentPlugin>
{
    public:
        static constexpr bool is_unique = true;

        ProcessComponent(
                Process::ProcessModel& proc,
                DocumentPlugin& doc,
                const Id<iscore::Component>& id,
                const QString& name,
                QObject* parent);

        virtual ~ProcessComponent();

        virtual void makeStream(const Context& ctx) = 0;
        AudioStream getStream() const
        { return m_stream; }

        virtual bool hasInput() const = 0;
        virtual bool hasOutput() const = 0;

    protected:
        AudioStream m_stream;
};

/// Utility class
template<typename Process_T,
         bool Input,
         bool Output>
class ProcessComponent_T : public ProcessComponent
{
    public:
        using ProcessComponent::ProcessComponent;

        const Process_T& process() const
        { return static_cast<const Process_T&>(ProcessComponent::process()); }

        bool hasInput() const override
        { return Input; }
        bool hasOutput() const override
        { return Output; }
};


class ISCORE_PLUGIN_AUDIO_EXPORT ProcessComponentFactory :
        public iscore::GenericComponentFactory<
            Process::ProcessModel,
            DocumentPlugin,
            ProcessComponentFactory>
{
        ISCORE_ABSTRACT_FACTORY_DECL(
                ProcessComponentFactory,
                "19b6c620-9beb-4271-8a2c-8b34a3c64deb")
    public:
        virtual ~ProcessComponentFactory();
        virtual ProcessComponent* make(
                Process::ProcessModel& proc,
                DocumentPlugin& doc,
                const Id<iscore::Component>&,
                QObject* paren_objt) const = 0;
};

using ProcessComponentFactoryList =
    iscore::GenericComponentFactoryList<
            Process::ProcessModel,
            DocumentPlugin,
            ProcessComponentFactory>;

template<
        typename ProcessComponent_T,
        typename Process_T>
class ProcessComponentFactory_T : public ProcessComponentFactory
{
    public:
        using ProcessComponentFactory::ProcessComponentFactory;

        bool matches(
                Process::ProcessModel& p,
                const Audio::AudioStreamEngine::DocumentPlugin&) const final override
        {
            return dynamic_cast<Process_T*>(&p);
        }

        ProcessComponent* make(
                Process::ProcessModel& proc,
                DocumentPlugin& doc,
                const Id<iscore::Component>& id,
                QObject* paren_objt) const final override
        {
            return new ProcessComponent_T{static_cast<Process_T&>(proc), doc, id, paren_objt};
        }
};
}
}

#define AUDIO_PROCESS_COMPONENT_FACTORY(FactoryName, Uuid, ProcessComponent, Process) \
class FactoryName final : \
        public Audio::AudioStreamEngine::ProcessComponentFactory_T<ProcessComponent, Process> \
{ \
        ISCORE_CONCRETE_FACTORY_DECL(Uuid)  \
};
