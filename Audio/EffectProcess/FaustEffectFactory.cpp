#include "FaustEffectFactory.hpp"
#include "FaustEffectModel.hpp"
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
        const Id<EffectModel>& id,
        QObject* parent) const
{
    return new FaustEffectModel{info, id, parent};
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
