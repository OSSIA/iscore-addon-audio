#pragma once
#include <Audio/EffectProcess/EffectModel.hpp>
#include <QJsonDocument>
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
        ISCORE_SERIALIZE_FRIENDS(FaustEffectModel, DataStream)
        ISCORE_SERIALIZE_FRIENDS(FaustEffectModel, JSONObject)
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
                Deserializer<Impl>& vis,
                QObject* parent) :
            EffectModel{vis, parent}
        {
            vis.writeTo(*this);
            init();
        }

        FaustEffectModel* clone(
                const Id<EffectModel>& newId,
                QObject* parent) const override;


        QString title() const override;

        const QString& text() const
        {
            return m_text;
        }

        void setText(const QString& txt);

        UuidKey<EffectFactory> concreteFactoryKey() const override
        {
            return "5354c61a-1649-4f59-b952-5c2f1b79c1bd";
        }

        void serialize_impl(const VisitorVariant& vis) const override;


    signals:
        void textChanged();

    private:
        void init();
        void reload();
        QString m_text;
        QString m_name;
};
}
}
