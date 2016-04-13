#include "MissingEffectModel.hpp"
namespace Audio
{
namespace Effect
{

MissingEffectModel::MissingEffectModel(
        const QByteArray& MissingProgram,
        const Id<EffectModel>& id,
        QObject* parent):
    EffectModel{id, parent}
{
    ISCORE_TODO;
}

MissingEffectModel::MissingEffectModel(
        const MissingEffectModel& source,
        const Id<EffectModel>& id,
        QObject* parent):
    EffectModel{id, parent}
{
    ISCORE_TODO;
}

MissingEffectModel*MissingEffectModel::clone(const Id<EffectModel>& newId, QObject* parent) const
{
    ISCORE_TODO;
    return nullptr;
}

}
}
