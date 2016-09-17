#pragma once
#include <Process/GenericProcessFactory.hpp>
#include <Process/WidgetLayer/WidgetProcessFactory.hpp>
#include <Audio/MixProcess/MixProcessModel.hpp>
#include <Audio/MixProcess/MixProcessMetadata.hpp>
#include <Audio/Inspector/MixInspector.hpp>

namespace Audio
{
namespace Mix
{
using Layer = Process::LayerModel_T<ProcessModel>;
}
}
LAYER_METADATA(
        ,
        Audio::Mix::Layer,
        "c11e74f1-93a0-483c-bcb8-1984ef777c8b",
        "MixLayer",
        "MixLayer"
        )


namespace Audio
{
namespace Mix
{
using ProcessFactory = Process::GenericProcessModelFactory<Mix::ProcessModel>;
using LayerFactory = WidgetLayer::LayerFactory<Mix::ProcessModel, Mix::Layer, MixWidget>;
}
}
