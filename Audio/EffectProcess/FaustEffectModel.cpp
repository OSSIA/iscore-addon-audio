#include "FaustEffectModel.hpp"
namespace Audio
{
namespace Effect
{

FaustEffectModel::FaustEffectModel(
        const QString& faustProgram,
        const Id<EffectModel>& id,
        QObject* parent):
    EffectModel{id, parent}
{
    ISCORE_TODO;
}

FaustEffectModel::FaustEffectModel(
        const FaustEffectModel& source,
        const Id<EffectModel>& id,
        QObject* parent):
    EffectModel{id, parent}
{
    ISCORE_TODO;
}

FaustEffectModel*FaustEffectModel::clone(const Id<EffectModel>& newId, QObject* parent) const
{
    ISCORE_TODO;
    return nullptr;
}

}
}
