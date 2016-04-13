#pragma once
#include <Audio/EffectProcess/EffectModel.hpp>

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
};
}
}
