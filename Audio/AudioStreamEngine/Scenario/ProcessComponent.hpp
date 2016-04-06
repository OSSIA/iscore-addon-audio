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
        ProcessComponent(
                Process::ProcessModel& proc,
                const Id<iscore::Component>& id,
                const QString& name,
                QObject* parent);

        virtual ~ProcessComponent();

    private:
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
}
}
