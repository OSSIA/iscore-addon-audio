#pragma once
#include <iscore/plugins/customfactory/FactoryInterface.hpp>
#include <iscore/serialization/VisitorCommon.hpp>
#include <iscore_plugin_audio_export.h>

namespace Audio
{
namespace Effect
{
class EffectModel;

class ISCORE_PLUGIN_AUDIO_EXPORT EffectFactory :
        public iscore::AbstractFactory<EffectFactory>
{
        ISCORE_ABSTRACT_FACTORY_DECL(
                EffectFactory,
                "3ffe0073-dfe0-4a7f-862f-220380ebcf08")
    public:
        virtual ~EffectFactory();

        virtual QString prettyName() const = 0; // VST, FaUST, etc...

        // Model
        virtual EffectModel* makeModel(
                const QString& info, // plugin name ? faust code ? dll location ?
                const Id<EffectModel>&,
                QObject* parent) const = 0;

        virtual EffectModel* load(
                const VisitorVariant& data,
                QObject* parent) const = 0;
};

class ISCORE_PLUGIN_AUDIO_EXPORT EffectFactoryList final :
        public iscore::ConcreteFactoryList<EffectFactory>
{
    public:
        using object_type = Audio::Effect::EffectModel;

};
}
}
Q_DECLARE_METATYPE(UuidKey<Audio::Effect::EffectFactory>)
