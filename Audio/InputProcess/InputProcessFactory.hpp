#pragma once
#include <Process/GenericProcessFactory.hpp>
#include <Audio/InputProcess/InputProcessModel.hpp>
#include <Audio/InputProcess/InputProcessMetadata.hpp>

namespace Audio
{
namespace Input
{
using ProcessFactory = Process::GenericProcessModelFactory<Input::ProcessModel>;
using LayerFactory = Process::GenericDefaultLayerFactory<Input::ProcessModel>;
}
}
