#pragma once
#include <Process/GenericProcessFactory.hpp>
#include <Audio/ReturnProcess/ReturnProcessModel.hpp>
#include <Audio/ReturnProcess/ReturnProcessMetadata.hpp>

namespace Audio
{
namespace Return
{
using Layer = Process::LayerModel_T<ProcessModel>;
}
}
LAYER_METADATA(
        ,
        Audio::Return::Layer,
        "b06172a1-342c-47c1-9223-296dff6b6a60",
        "ReturnLayer",
        "ReturnLayer"
        )

namespace Audio
{
namespace Return
{
using ProcessFactory = Process::GenericProcessModelFactory<Return::ProcessModel>;
using LayerFactory = Process::GenericDefaultLayerFactory<Return::Layer>;
}
}
