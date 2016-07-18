#pragma once
#include <Audio/EffectProcess/Effect/EffectFactory.hpp>
#include <Process/ModelMetadata.hpp>

#include <iscore/component/Component.hpp>
#include <iscore/tools/IdentifiedObject.hpp>
#include <iscore/plugins/customfactory/SerializableInterface.hpp>
#include <iscore_plugin_audio_export.h>

#include <LibAudioStreamMC++.h>
namespace Audio
{
namespace Effect
{
/**
 * @brief The EffectModel class
 *
 * Abstract class for an effect instance.
 * A concrete example class is FaustEffectModel,
 * which represents a Faust effect; each instance
 * of FaustEffectModel could be a different effect (e.g.
 * reverb, distorsion, etc.)
 */
class ISCORE_PLUGIN_AUDIO_EXPORT EffectModel :
        public IdentifiedObject<EffectModel>,
        public iscore::SerializableInterface<EffectFactory>
{
        Q_OBJECT
        ISCORE_SERIALIZE_FRIENDS(Audio::Effect::EffectModel, DataStream)
        ISCORE_SERIALIZE_FRIENDS(Audio::Effect::EffectModel, JSONObject)
    public:
        iscore::Components components;
        ModelMetadata metadata;

        EffectModel(
                const Id<EffectModel>&,
                QObject* parent);

        EffectModel(
                const EffectModel& source,
                const Id<EffectModel>&,
                QObject* parent);

        template<typename Impl>
        EffectModel(
                Deserializer<Impl>& vis,
                QObject* parent) :
            IdentifiedObject<EffectModel>{vis, parent}
        {
            vis.writeTo(*this);
        }

        virtual ~EffectModel();

        virtual EffectModel* clone(
                const Id<EffectModel>& newId,
                QObject* parent) const = 0;

        AudioEffect effect() const
        { return m_effect; }

    protected:
        AudioEffect m_effect;
};
}
}

#define EFFECT_METADATA(Export, Model, Uuid, ObjectKey, PrettyName) \
    MODEL_METADATA(Export, Audio::Effect::EffectFactory, Model, Uuid, ObjectKey, PrettyName)

#define EFFECT_METADATA_IMPL(Effect_T) \
UuidKey<Audio::Effect::EffectFactory> concreteFactoryKey() const final override \
{ \
    return Metadata<ConcreteFactoryKey_k, Effect_T>::get(); \
} \
 \
void serialize_impl(const VisitorVariant& vis) const final override \
{ \
    serialize_dyn(vis, *this); \
} \
\
Effect_T* clone( \
    const Id<Audio::Effect::EffectModel>& newId, \
    QObject* newParent) const final override\
{ \
   return new Effect_T{*this, newId, newParent}; \
}



Q_DECLARE_METATYPE(Id<Audio::Effect::EffectModel>)
