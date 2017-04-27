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
using ProcessFactory = Process::GenericProcessModelFactory<Mix::ProcessModel>;
using LayerFactory = WidgetLayer::LayerFactory<Mix::ProcessModel, MixWidget>;
}
}
