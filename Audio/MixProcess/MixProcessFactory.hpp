#pragma once
#include <Process/WidgetLayer/WidgetProcessFactory.hpp>
#include <Audio/MixProcess/MixProcessModel.hpp>
#include <Audio/MixProcess/MixProcessMetadata.hpp>
#include <Audio/Inspector/MixInspector.hpp>
namespace Audio
{
namespace Mix
{
using ProcessFactory = WidgetLayer::ProcessFactory<Mix::ProcessModel, MixWidget>;
}
}
