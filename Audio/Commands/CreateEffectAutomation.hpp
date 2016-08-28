#pragma once
#include <iscore/command/AggregateCommand.hpp>
#include <Audio/Commands/AudioCommandFactory.hpp>

namespace Audio
{
namespace Commands
{
class CreateEffectAutomation final : public iscore::AggregateCommand
{
        ISCORE_COMMAND_DECL(Audio::CommandFactoryName(), CreateEffectAutomation, "Create effect automation")
};
}
}
