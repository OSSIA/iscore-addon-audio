#pragma once
#include <Audio/EffectProcess/Effect/EffectFactory.hpp>

namespace Audio
{
namespace Effect
{
/**
 * @brief The FaustEffectFactory class
 *
 * Creates or load a Faust effect.
 * A Faust effect can be either given directly in
 * text form, or as a .dsp file;
 * this logic is already handled by the LibAudioStream.
 *
 */
class FaustEffectFactory final : public EffectFactory
{
    public:
        ISCORE_CONCRETE_FACTORY("5354c61a-1649-4f59-b952-5c2f1b79c1bd")

        QString prettyName() const override;

        EffectModel* make(
                const QString& info,
                const Id<EffectModel>&,
                QObject* parent) const override;

        EffectModel* load(
                const VisitorVariant& data,
                QObject* parent) const override;

};
}
}
