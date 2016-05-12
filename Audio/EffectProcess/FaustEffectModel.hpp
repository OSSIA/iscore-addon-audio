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
        }

        FaustEffectModel* clone(
                const Id<EffectModel>& newId,
                QObject* parent) const override;


        QString title() const override {
            return "Faust";
        }

        const QString& text() const
        {
            return m_text;
        }

        void setText(const QString& txt)
        {
            m_text = txt;
            auto fx_text = m_text.toLocal8Bit();
            m_effect = MakeFaustAudioEffect(fx_text, "", "");

            if(m_effect)
            {
                auto json = GetJsonEffect(m_effect);
                QJsonParseError err;
                auto qjs = QJsonDocument::fromJson(json, &err);
                if(err.error == QJsonParseError::NoError)
                {
                    qDebug() << qjs;
                }
                else
                {
                    qDebug() << err.errorString();
                }
            }

            emit textChanged();
        }

        UuidKey<EffectFactory> concreteFactoryKey() const override
        {
            return "3db761db-dcc4-4036-ac10-18dc786239c0";
        }
    signals:
        void textChanged();

    private:
        QString m_text;
};
}
}
