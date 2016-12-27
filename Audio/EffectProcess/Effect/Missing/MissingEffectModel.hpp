#pragma once
#include <Audio/EffectProcess/Effect/EffectModel.hpp>

namespace Audio
{
namespace Effect
{
/**
 * @brief The MissingEffectModel class
 *
 * Used when a plug-in cannot be loaded for some reason.
 * This should take care of keeping the same memory layout
 * so that if reloading with the plug-in now available, everything loads
 * correctly.
 * This means that plug-in data should be encoded in QByteArrays in base64 or
 * something similar even in the JSON.
 */
class MissingEffectModel :
        public EffectModel
{
        Q_OBJECT
        ISCORE_SERIALIZE_FRIENDS
    public:
        MissingEffectModel(
                const QByteArray& data,
                const Id<EffectModel>&,
                QObject* parent);

        MissingEffectModel(
                const MissingEffectModel& source,
                const Id<EffectModel>&,
                QObject* parent);

        template<typename Impl>
        MissingEffectModel(
                Impl& vis,
                QObject* parent) :
            EffectModel{vis, parent}
        {
            vis.writeTo(*this);
        }

        MissingEffectModel* clone(
                const Id<EffectModel>& newId,
                QObject* parent) const override;
};
}
}
