#pragma once
#include <Process/GenericProcessFactory.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Audio/EffectProcess/EffectProcessMetadata.hpp>

namespace Audio
{
namespace Effect
{
using ProcessFactory = Process::GenericDefaultProcessFactory<Effect::ProcessModel>;
}
}
