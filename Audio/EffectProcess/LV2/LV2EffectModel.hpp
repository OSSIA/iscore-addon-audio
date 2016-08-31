#pragma once
#include <Audio/EffectProcess/Effect/EffectModel.hpp>
#include <lilv/lilvmm.hpp>
#include <QJsonDocument>

namespace Audio
{
namespace Effect
{
class LV2EffectModel;
}
}
EFFECT_METADATA(, Audio::Effect::LV2EffectModel, "fd5243ba-70b5-4164-b44a-ecb0dcdc0494", "LV2", "LV2")
namespace Audio
{
namespace Effect
{



/** LV2 effect model.
 * Should contain an effect, maybe instantiated with
 * LibAudioStream's MakeLV2AudioEffect
 * Cloning can be done with MakeCopyEffect.
 */
class LV2EffectModel :
        public EffectModel
{
        Q_OBJECT
        ISCORE_SERIALIZE_FRIENDS(LV2EffectModel, DataStream)
        ISCORE_SERIALIZE_FRIENDS(LV2EffectModel, JSONObject)
        MODEL_METADATA_IMPL(LV2EffectModel)
    public:
        LV2EffectModel(
                const QString& name,
                const Id<EffectModel>&,
                QObject* parent);

        LV2EffectModel(
                const LV2EffectModel& source,
                const Id<EffectModel>&,
                QObject* parent);

        template<typename Impl>
        LV2EffectModel(
                Deserializer<Impl>& vis,
                QObject* parent) :
            EffectModel{vis, parent}
        {
            vis.writeTo(*this);
            reload();
        }

        const QString& effect() const
        { return m_effectPath; }

        void setEffect(const QString& s)
        { m_effectPath = s; }

        const LilvPlugin* plugin{};
    private:
        void reload();
        QString m_effectPath;
};
using LV2EffectFactory = EffectFactory_T<LV2EffectModel>;
}
}
