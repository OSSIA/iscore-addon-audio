#pragma once
#include <Process/GenericProcessFactory.hpp>
#include <Audio/ReturnProcess/ReturnProcessModel.hpp>
#include <Audio/ReturnProcess/ReturnProcessMetadata.hpp>

namespace Audio
{
namespace Return
{
using ProcessFactory = Process::GenericDefaultProcessFactory<Return::ProcessModel>;
}
}
