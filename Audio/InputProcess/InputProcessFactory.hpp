#pragma once
#include <Process/GenericProcessFactory.hpp>
#include <Audio/InputProcess/InputProcessModel.hpp>
#include <Audio/InputProcess/InputProcessMetadata.hpp>

namespace Audio
{
namespace Input
{
using ProcessFactory = Process::GenericDefaultProcessFactory<Input::ProcessModel>;
}
}
