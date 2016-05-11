#pragma once
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <Process/Process.hpp>
#include <iscore/component/Component.hpp>
#include <iscore/component/ComponentFactory.hpp>
#include <iscore_plugin_audio_export.h>

// TODO clean me up
namespace Audio
{
namespace AudioStreamEngine
{


class ISCORE_PLUGIN_AUDIO_EXPORT ProcessComponent : public iscore::Component
{
    public:
        const Process::ProcessModel& process;

        ProcessComponent(
                Process::ProcessModel& proc,
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
                const Id<iscore::Component>&,
                Process::ProcessModel& proc,
                const DocumentPlugin& doc,
                const iscore::DocumentContext& ctx,
                QObject* paren_objt) const = 0;
};

using ProcessComponentFactoryList =
    iscore::GenericComponentFactoryList<
            Process::ProcessModel,
            DocumentPlugin,
            ProcessComponentFactory>;

/// Utility class
template<typename Process_T,
         bool Input,
         bool Output>
class ProcessComponent_T : public ProcessComponent
{
    public:
        using ProcessComponent::ProcessComponent;

        const Process_T& process() const
        { return static_cast<const Process_T&>(ProcessComponent::process); }

        bool hasInput() const override
        { return Input; }
        bool hasOutput() const override
        { return Output; }
};
}
}
