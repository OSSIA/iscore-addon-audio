#include "LV2EffectModel.hpp"
#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>
namespace Audio
{
namespace Effect
{

LV2EffectModel::LV2EffectModel(
        const QString& path,
        const Id<EffectModel>& id,
        QObject* parent):
    EffectModel{id, parent}
{
    init();
}

LV2EffectModel::LV2EffectModel(
        const LV2EffectModel& source,
        const Id<EffectModel>& id,
        QObject* parent):
    EffectModel{id, parent}
{
    init();
}

LV2EffectModel*LV2EffectModel::clone(const Id<EffectModel>& newId, QObject* parent) const
{
    return new LV2EffectModel{*this, newId, parent};
}

QString LV2EffectModel::title() const
{
    return tr("LV2");
}

}
}
