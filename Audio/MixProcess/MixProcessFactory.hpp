#pragma once
#include <Process/GenericProcessFactory.hpp>
#include <Audio/MixProcess/MixProcessModel.hpp>
#include <Audio/MixProcess/MixProcessMetadata.hpp>

namespace Audio
{
namespace Mix
{
using ProcessFactory = Process::GenericDefaultProcessFactory<Mix::ProcessModel>;
}
}
