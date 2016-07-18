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
        const VisitorVariant& vis,
        QObject* parent) const
{
    return deserialize_dyn(vis, [&] (auto&& deserializer)
    { return new FaustEffectModel{deserializer, parent};});
}

}
}
