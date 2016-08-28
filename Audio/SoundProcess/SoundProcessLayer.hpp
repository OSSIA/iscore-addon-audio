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

DEFAULT_MODEL_METADATA(Audio::Sound::Layer, "SoundLayer")
