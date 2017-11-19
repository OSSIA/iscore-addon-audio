#pragma once
#include <Audio/EffectProcess/Effect/EffectFactory.hpp>
#include <score/model/Entity.hpp>

#include <score/model/Component.hpp>
#include <score/model/IdentifiedObject.hpp>
#include <score/plugins/customfactory/SerializableInterface.hpp>
#include <score_plugin_audio_export.h>

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
class SCORE_PLUGIN_AUDIO_EXPORT EffectModel :
        public score::Entity<EffectModel>,
        public score::SerializableInterface<EffectFactory>
{
        Q_OBJECT
        SCORE_SERIALIZE_FRIENDS
    public:
        EffectModel(
                const Id<EffectModel>&,
                QObject* parent);

        EffectModel(
                const EffectModel& source,
                const Id<EffectModel>&,
                QObject* parent);

        template<typename Impl>
        EffectModel(
                Impl& vis,
                QObject* parent) :
            Entity{vis, parent}
        {
            vis.writeTo(*this);
        }

        virtual ~EffectModel();

        virtual EffectModel* clone(
                const Id<EffectModel>& newId,
                QObject* parent) const = 0;

        AudioEffect effect() const
        { return m_effect; }

        std::vector<float> getParams() const;
        std::vector<float>& savedParams() const
        { return m_params; }

    signals:
        void effectChanged() const;

    protected:
        void saveParams() const;
        void restoreParams();
        mutable std::vector<float> m_params;
        AudioEffect m_effect;
};
}
}

#define EFFECT_METADATA(Export, Model, Uuid, ObjectKey, PrettyName) \
    MODEL_METADATA(Export, Audio::Effect::EffectFactory, Model, Uuid, ObjectKey, PrettyName)

Q_DECLARE_METATYPE(Id<Audio::Effect::EffectModel>)

TR_TEXT_METADATA(, Audio::Effect::EffectModel, PrettyName_k, "Effect")
