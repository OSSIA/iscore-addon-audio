#pragma once
#include <Process/LayerModel.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>
namespace Audio
{
namespace Sound
{
using Layer = Process::LayerModel_T<ProcessModel>;
}
}

LAYER_METADATA(
        ,
        Audio::Sound::Layer,
        "5b381b4a-4e3f-4450-92a5-8f264bb10e59",
        "SoundLayer",
        "SoundLayer"
        )
