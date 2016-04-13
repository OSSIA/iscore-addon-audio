#include "FaustEffectFactory.hpp"
namespace Audio
{
namespace Effect
{

QString FaustEffectFactory::prettyName() const
{
    return "Faust";
}

EffectModel*FaustEffectFactory::makeModel(
        const QString& info,
        const Id<EffectModel>&,
        QObject* parent) const
{
    ISCORE_TODO;
    return nullptr;
}

EffectModel*FaustEffectFactory::load(
        const VisitorVariant& data,
        QObject* parent) const
{
    ISCORE_TODO;
    return nullptr;
}

}
}
