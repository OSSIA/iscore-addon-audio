#pragma once
#include <TExpAudioMixer.h>
#include <set>

// We save the original commands in order to reset them when in a loop.
class TGroupAudioMixer final : public TExpAudioMixer
{
    public:
        using TExpAudioMixer::TExpAudioMixer;
        struct CommandPack
        {
                TCommandPtr fCommand;
                mutable audio_frame_t fStart;
                mutable audio_frame_t fStop;

                friend bool operator==(const CommandPack& lhs, TCommandPtr rhs)
                {
                    return lhs.fCommand == rhs;
                }
                friend bool operator!=(const CommandPack& lhs, TCommandPtr rhs)
                {
                    return lhs.fCommand != rhs;
                }
                friend bool operator<(const CommandPack& lhs, TCommandPtr rhs)
                {
                    return lhs.fCommand < rhs;
                }
                friend bool operator==(TCommandPtr lhs, const CommandPack& rhs)
                {
                    return lhs == rhs.fCommand;
                }
                friend bool operator!=(TCommandPtr lhs, const CommandPack& rhs)
                {
                    return lhs != rhs.fCommand;
                }
                friend bool operator<(TCommandPtr lhs, const CommandPack& rhs)
                {
                    return lhs < rhs.fCommand;
                }
                friend bool operator<(const CommandPack& lhs, const CommandPack& rhs)
                {
                    return lhs.fCommand < rhs.fCommand;
                }
        };

        std::set<CommandPack, std::less<>>
            fSavedStreamCommands,
            fSavedControlCommands;

        void Reset();

    private:
        void AddStreamCommand(TCommandPtr command) override;
        void StopStreamCommand(TStreamCommandPtr command, SymbolicDate date) override;
        void RemoveStreamCommand(TCommandPtr command) override;


        void AddControlCommand(TCommandPtr command) override;
        void RemoveControlCommand(TCommandPtr command) override;
};
