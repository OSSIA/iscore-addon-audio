#pragma once
#include <Audio/EffectProcess/EffectFactory.hpp>

namespace Audio
{
namespace Effect
{
class FaustEffectFactory final : public EffectFactory
{
    public:
        ISCORE_CONCRETE_FACTORY_DECL("5354c61a-1649-4f59-b952-5c2f1b79c1bd")

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
