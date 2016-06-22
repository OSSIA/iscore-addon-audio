#pragma once
#include <Audio/EffectProcess/EffectFactory.hpp>

namespace Audio
{
namespace Effect
{
/**
 * @brief The LV2EffectFactory class
 *
 * Creates or load a LV2 effect.
 * A LV2 effect is a QString to the effect file.
 *
 */
class LV2EffectFactory final : public EffectFactory
{
    public:
        ISCORE_CONCRETE_FACTORY_DECL("fd5243ba-70b5-4164-b44a-ecb0dcdc0494")

        QString prettyName() const override;

        EffectModel* makeModel(
                const QString& info,
                const Id<EffectModel>&,
                QObject* parent) const override;

        EffectModel* load(
                const VisitorVariant& data,
                QObject* parent) const override;

};
}
}
