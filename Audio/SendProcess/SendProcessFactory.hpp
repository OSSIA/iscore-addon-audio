#pragma once
#include <Process/GenericProcessFactory.hpp>
#include <Audio/SendProcess/SendProcessModel.hpp>
#include <Audio/SendProcess/SendProcessMetadata.hpp>

namespace Audio
{
namespace Send
{
using Layer = Process::LayerModel_T<ProcessModel>;
}
}
LAYER_METADATA(
        ,
        Audio::Send::Layer,
        "bd1c3698-24bf-4f3c-9044-ea12e4bd9f8b",
        "SendLayer",
        "SendLayer"
        )

namespace Audio
{
namespace Send
{
using ProcessFactory = Process::GenericProcessModelFactory<Send::ProcessModel>;
using LayerFactory = Process::GenericDefaultLayerFactory<Send::Layer>;
}
}
