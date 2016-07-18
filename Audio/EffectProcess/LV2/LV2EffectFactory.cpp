#include "LV2EffectFactory.hpp"
#include "LV2EffectModel.hpp"
namespace Audio
{
namespace Effect
{

QString LV2EffectFactory::prettyName() const
{
    return "LV2";
}

EffectModel*LV2EffectFactory::make(
        const QString& info,
        const Id<EffectModel>& id,
        QObject* parent) const
{
    return new LV2EffectModel{info, id, parent};
}

EffectModel*LV2EffectFactory::load(
        const VisitorVariant& data,
        QObject* parent) const
{
    ISCORE_TODO;
    return nullptr;
}

}
}
