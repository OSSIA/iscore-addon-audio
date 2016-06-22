#pragma once
#include <Audio/EffectProcess/EffectModel.hpp>
#include <lilv/lilvmm.hpp>
#include <QJsonDocument>
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
    public:
        LV2EffectModel(
                const QString& faustProgram,
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
            init();
        }

        LV2EffectModel* clone(
                const Id<EffectModel>& newId,
                QObject* parent) const override;


        QString title() const override;

        const QString& path() const
        { return m_path; }

        void setPath(const QString& s)
        { m_path = s; }
        UuidKey<EffectFactory> concreteFactoryKey() const override
        {
            return "fd5243ba-70b5-4164-b44a-ecb0dcdc0494";
        }
    signals:
        void textChanged();

    private:
        void init();
        void reload();
        QString m_path;
};
}
}
