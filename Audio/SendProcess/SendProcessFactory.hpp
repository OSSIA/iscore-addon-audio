#pragma once
#include <Process/GenericProcessFactory.hpp>
#include <Audio/SendProcess/SendProcessModel.hpp>
#include <Audio/SendProcess/SendProcessMetadata.hpp>

namespace Audio
{
namespace Send
{
using ProcessFactory = Process::GenericDefaultProcessFactory<Send::ProcessModel>;
}
}
