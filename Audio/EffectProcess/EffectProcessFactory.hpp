#pragma once
#include <Process/WidgetLayer/WidgetProcessFactory.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Audio/EffectProcess/EffectProcessMetadata.hpp>
#include <Audio/EffectProcess/Effect/Widgets/EffectListWidget.hpp>
namespace Audio
{
namespace Effect
{
using ProcessFactory = WidgetLayer::ProcessFactory<Effect::ProcessModel, EffectListWidget>;
}
}
