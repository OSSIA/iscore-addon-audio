#pragma once
#include <Process/GenericProcessFactory.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>
#include <Audio/SoundProcess/SoundProcessMetadata.hpp>
#include <Audio/SoundProcess/SoundProcessPresenter.hpp>
#include <Audio/SoundProcess/SoundProcessView.hpp>

namespace Audio
{
namespace Sound
{
using ProcessFactory = Process::GenericProcessFactory<
    Sound::ProcessModel,
    Sound::LayerModel,
    Sound::LayerPresenter,
    Sound::LayerView>;
}
}
