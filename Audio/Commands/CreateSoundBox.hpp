#pragma once
#include <Audio/Commands/CreateSoundBox.hpp>
#include <iscore/command/AggregateCommand.hpp>
namespace Audio
{
namespace Commands
{

class CreateSoundBoxMacro final : public iscore::AggregateCommand
{
        ISCORE_COMMAND_DECL(Audio::CommandFactoryName(), CreateSoundBoxMacro, "Add sounds in a box")
};

class CreateSoundBoxesMacro final : public iscore::AggregateCommand
{
        ISCORE_COMMAND_DECL(Audio::CommandFactoryName(), CreateSoundBoxesMacro, "Add sounds in sequence")
};

}
}
