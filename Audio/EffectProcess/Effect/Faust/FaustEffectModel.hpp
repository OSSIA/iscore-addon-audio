#pragma once
#include <Audio/EffectProcess/Effect/EffectModel.hpp>
#include <Audio/EffectProcess/Effect/EffectFactory.hpp>
#include <QJsonDocument>
namespace Audio
{
namespace Effect
{
class FaustEffectModel;
}
}
EFFECT_METADATA(, Audio::Effect::FaustEffectModel, "5354c61a-1649-4f59-b952-5c2f1b79c1bd", "Faust", "Faust")

namespace Audio
{
namespace Effect
{
/** Faust effect model.
 * Should contain an effect, maybe instantiated with
 * LibAudioStream's MakeFaustAudioEffect
 * Cloning can be done with MakeCopyEffect.
 */
class FaustEffectModel :
        public EffectModel
{
        Q_OBJECT
        SCORE_SERIALIZE_FRIENDS
        MODEL_METADATA_IMPL(FaustEffectModel)

    public:
        FaustEffectModel(
                const QString& faustProgram,
                const Id<EffectModel>&,
                QObject* parent);

        FaustEffectModel(
                const FaustEffectModel& source,
                const Id<EffectModel>&,
                QObject* parent);

        template<typename Impl>
        FaustEffectModel(
                Impl& vis,
                QObject* parent) :
            EffectModel{vis, parent}
        {
            vis.writeTo(*this);
            init();
        }

        const QString& text() const
        {
            return m_text;
        }

        void setText(const QString& txt);

    private:
        void init();
        void reload();
        QString m_text;
};

using FaustEffectFactory = EffectFactory_T<FaustEffectModel>;
}
}
