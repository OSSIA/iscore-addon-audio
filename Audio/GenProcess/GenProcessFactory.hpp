#pragma once
#include <Process/GenericProcessFactory.hpp>
#include <Audio/GenProcess/GenProcessModel.hpp>
#include <Audio/GenProcess/GenProcessMetadata.hpp>

namespace Audio
{
namespace Gen
{
using ProcessFactory = Process::GenericDefaultProcessFactory<Gen::ProcessModel>;
}
}
