#include "EffectModel.hpp"
namespace Audio
{
namespace Effect
{
EffectModel::EffectModel(
        const Id<EffectModel>& id,
        QObject* parent):
    IdentifiedObject{id, staticMetaObject.className(), parent}
{

}

EffectModel::EffectModel(
        const EffectModel& source,
        const Id<EffectModel>& id,
        QObject* parent):
    IdentifiedObject{id, staticMetaObject.className(), parent}
{
    ISCORE_TODO;
}

EffectModel::~EffectModel()
{

}

}
}
