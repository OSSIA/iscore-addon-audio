#pragma once
#include <score/command/AggregateCommand.hpp>
#include <Audio/Commands/AudioCommandFactory.hpp>

namespace Audio
{
namespace Commands
{
class CreateEffectAutomation final : public score::AggregateCommand
{
        SCORE_COMMAND_DECL(Audio::CommandFactoryName(), CreateEffectAutomation, "Create effect automation")
};
}
}
