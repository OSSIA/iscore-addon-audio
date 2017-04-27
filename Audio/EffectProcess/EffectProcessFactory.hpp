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
using ProcessFactory = Process::GenericProcessModelFactory<Effect::ProcessModel>;
using LayerFactory = WidgetLayer::LayerFactory<Effect::ProcessModel, EffectListWidget>;
}
}
