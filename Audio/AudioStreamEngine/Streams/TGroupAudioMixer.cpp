#include "TGroupAudioMixer.hpp"

void TGroupAudioMixer::Reset()
{
    for(auto pair : fSavedStreamCommands)
    {
        TExpAudioMixer::RemoveStreamCommand(pair.fCommand);

        auto cmd = dynamic_cast<TStreamCommand*>(pair.fCommand.getPointer());
        if(cmd)
        {
            cmd->fStartDate->setDate(pair.fStart);
            cmd->fStopDate->setDate(pair.fStop);
            cmd->fStream->Reset();
            cmd->fStream->SetPos(0);
            cmd->fPos = 0;
        }
        TExpAudioMixer::AddStreamCommand(pair.fCommand);
    }

    for(auto pair : fSavedControlCommands)
    {
        TExpAudioMixer::RemoveControlCommand(pair.fCommand);

        pair.fCommand->fStartDate->setDate(pair.fStart);

        TExpAudioMixer::AddControlCommand(pair.fCommand);
    }

    SetPos(0);
}

void TGroupAudioMixer::AddStreamCommand(TCommandPtr command)
{
    TExpAudioMixer::AddStreamCommand(command);
    fSavedStreamCommands.insert({command, command->GetDate(), LONG_MAX});
}

void TGroupAudioMixer::StopStreamCommand(TStreamCommandPtr command, SymbolicDate date)
{
    TExpAudioMixer::StopStreamCommand(command, date);

    auto it = fSavedStreamCommands.find(command);
    assert(it != fSavedStreamCommands.end());
    it->fStop = date->GetDate();
}

void TGroupAudioMixer::RemoveStreamCommand(TCommandPtr command)
{
    TExpAudioMixer::RemoveStreamCommand(command);
    fSavedStreamCommands.erase(fSavedStreamCommands.find(command));
}

void TGroupAudioMixer::AddControlCommand(TCommandPtr command)
{
    TExpAudioMixer::AddControlCommand(command);
    fSavedControlCommands.insert({command, command->GetDate(), LONG_MAX});
}

void TGroupAudioMixer::RemoveControlCommand(TCommandPtr command)
{
    TExpAudioMixer::RemoveControlCommand(command);
    fSavedControlCommands.erase(fSavedStreamCommands.find(command));
}
