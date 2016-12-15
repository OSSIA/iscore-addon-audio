#pragma once
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <Audio/AudioStreamEngine/AudioComponent.hpp>
#include <Process/Process.hpp>
#include <iscore/model/Component.hpp>
#include <iscore/model/ComponentFactory.hpp>
#include <iscore_plugin_audio_export.h>
#include <Scenario/Document/Components/ProcessComponent.hpp>
#include <iscore/plugins/customfactory/ModelFactory.hpp>
#include <Audio/AudioStreamEngine/AudioDependencyGraph.hpp>

// TODO clean me up
namespace Audio
{
namespace AudioStreamEngine
{
class ISCORE_PLUGIN_AUDIO_EXPORT ProcessComponent :
        public Scenario::ProcessComponent<Component>
{
        ABSTRACT_COMPONENT_METADATA(ProcessComponent, "19b6c620-9beb-4271-8a2c-8b34a3c64deb")
    public:
        static constexpr bool is_unique = true;

        ProcessComponent(
                Process::ProcessModel& proc,
                DocumentPlugin& doc,
                const Id<iscore::Component>& id,
                const QString& name,
                QObject* parent);

        virtual ~ProcessComponent();

        virtual optional<AudioGraphVertice> visit(AudioGraph& graph) override
        {
            // Covers most cases
            return boost::add_vertex(this, graph);
        }

        virtual QString prettyName() const override;
        virtual bool hasInput() const = 0;
        virtual bool hasOutput() const = 0;
};

/// Utility class
template<typename Process_T,
         bool Input,
         bool Output>
class ProcessComponent_T : public Scenario::GenericProcessComponent_T<ProcessComponent, Process_T>
{
    public:
        using Scenario::GenericProcessComponent_T<ProcessComponent, Process_T>::GenericProcessComponent_T;
        static const constexpr bool has_input = Input;
        static const constexpr bool has_output = Output;
        bool hasInput() const override
        { return Input; }
        bool hasOutput() const override
        { return Output; }
};


class ISCORE_PLUGIN_AUDIO_EXPORT ProcessComponentFactory :
        public iscore::GenericComponentFactory<
            Process::ProcessModel,
            DocumentPlugin,
            ProcessComponentFactory>,
        public iscore::GenericComponentFactory_Make<
            ProcessComponent,
            iscore::MakeArgs<
                Process::ProcessModel&,
                DocumentPlugin&,
                const Id<iscore::Component>&,
                QObject*>
        >
{
        ISCORE_ABSTRACT_COMPONENT_FACTORY(ProcessComponent)
    public:
        virtual ~ProcessComponentFactory();

        virtual bool hasInput() const = 0;
        virtual bool hasOutput() const = 0;
};

template<typename ProcessComponent_T>
class ProcessComponentFactory_T :
        public iscore::GenericComponentFactoryImpl<ProcessComponent_T, ProcessComponentFactory>
{
    public:
        ProcessComponent* make(
                Process::ProcessModel& proc,
                DocumentPlugin& doc,
                const Id<iscore::Component>& id,
                QObject* paren_objt) const final override
        {
            return new ProcessComponent_T{static_cast< typename ProcessComponent_T::model_type&>(proc), doc, id, paren_objt};
        }

        bool hasInput() const override
        { return ProcessComponent_T::has_input; }
        bool hasOutput() const override
        { return ProcessComponent_T::has_output; }
};


using ProcessComponentFactoryList =
    iscore::GenericComponentFactoryList<
            Process::ProcessModel,
            DocumentPlugin,
            ProcessComponentFactory>;

}
}
