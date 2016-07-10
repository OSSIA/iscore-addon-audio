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
class ProcessComponent_T : public Scenario::GenericProcessComponent_T<ProcessComponent, Process_T>
{
    public:
        using Scenario::GenericProcessComponent_T<ProcessComponent, Process_T>::GenericProcessComponent_T;

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
        ISCORE_ABSTRACT_FACTORY("19b6c620-9beb-4271-8a2c-8b34a3c64deb")
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

template<typename ProcessComponent_T>
class ProcessComponentFactory_T : public ProcessComponentFactory
{
    public:
        using model_type = typename ProcessComponent_T::model_type;
        using component_type = ProcessComponent_T;
        using ProcessComponentFactory::ProcessComponentFactory;

        static auto static_concreteFactoryKey()
        {
            return ProcessComponent_T::static_key().impl();
        }

        ConcreteFactoryKey concreteFactoryKey() const final override
        {
            return ProcessComponent_T::static_key().impl(); // Note : here there is a conversion between UuidKey<Component> and ConcreteFactoryKey
        }

        bool matches(
                Process::ProcessModel& p,
                const Audio::AudioStreamEngine::DocumentPlugin&) const final override
        {
            return dynamic_cast<model_type*>(&p);
        }

        ProcessComponent* make(
                Process::ProcessModel& proc,
                DocumentPlugin& doc,
                const Id<iscore::Component>& id,
                QObject* paren_objt) const final override
        {
            return new ProcessComponent_T{static_cast<model_type&>(proc), doc, id, paren_objt};
        }
};
}
}
