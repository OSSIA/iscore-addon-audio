#pragma once
#include <Process/GenericProcessFactory.hpp>
#include <Process/WidgetLayer/WidgetProcessFactory.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Audio/EffectProcess/EffectProcessMetadata.hpp>
#include <Audio/EffectProcess/Effect/Widgets/EffectListWidget.hpp>

namespace Audio
{
namespace Effect
{
using Layer = Process::LayerModel_T<ProcessModel>;
}
}
LAYER_METADATA(
        ,
        Audio::Effect::Layer,
        "74c1d096-deba-4351-beb4-14da74a042b0",
        "EffectLayer",
        "EffectLayer"
        )

namespace Audio
{
namespace Effect
{
using ProcessFactory = Process::GenericProcessModelFactory<Effect::ProcessModel>;
using LayerFactory = WidgetLayer::LayerFactory<Effect::ProcessModel, Effect::Layer, EffectListWidget>;
}
}
