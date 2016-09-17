#pragma once
#include <Process/GenericProcessFactory.hpp>
#include <Audio/InputProcess/InputProcessModel.hpp>
#include <Audio/InputProcess/InputProcessMetadata.hpp>

namespace Audio
{
namespace Input
{
using Layer = Process::LayerModel_T<ProcessModel>;
}
}
LAYER_METADATA(
        ,
        Audio::Input::Layer,
        "8ba3de3c-7e54-4cee-b3ba-1e4128b917e6",
        "InputLayer",
        "InputLayer"
        )

namespace Audio
{
namespace Input
{
using ProcessFactory = Process::GenericProcessModelFactory<Input::ProcessModel>;
using LayerFactory = Process::GenericDefaultLayerFactory<Input::Layer>;
}
}
