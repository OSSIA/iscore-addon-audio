#pragma once
#include <Process/GenericProcessFactory.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>
#include <Audio/SoundProcess/SoundProcessLayer.hpp>
#include <Audio/SoundProcess/SoundProcessMetadata.hpp>
#include <Audio/SoundProcess/SoundProcessPresenter.hpp>
#include <Audio/SoundProcess/SoundProcessView.hpp>
#include <Process/LayerModelPanelProxy.hpp>

namespace Audio
{
namespace Sound
{
using ProcessFactory = Process::GenericProcessModelFactory<Sound::ProcessModel>;
using LayerFactory = Process::GenericLayerFactory<
    Sound::ProcessModel,
    Sound::LayerPresenter,
    Sound::LayerView,
    Process::GraphicsViewLayerPanelProxy>;
}
}
