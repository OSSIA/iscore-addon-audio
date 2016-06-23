#pragma once

#include <3rdparty/libaudiostream/src/TAudioStream.h>
#include <3rdparty/libaudiostream/src/renderer/TAudioRenderer.h>
#include "TExpAudioMixer.h"
#include "TEffectAudioStream.h"

#include <set>
#include <iostream>
#include <memory>
#include <cmath>

struct TBufferManager
{
        float** fInputBuffer{};
        float** fOutputBuffer{};

        long fInput{};
        long fOutput{};
        long fBufferSize{};
        long fSampleRate{};

        TBufferManager() = default;

        TBufferManager(
                long inChan,
                long outChan,
                long bufferSize,
                long sampleRate):
            fInputBuffer{inChan > 0 ? new float*[inChan] : nullptr},
            fOutputBuffer{outChan > 0 ? new float*[outChan] : nullptr},
            fInput{inChan},
            fOutput{outChan},
            fBufferSize{bufferSize},
            fSampleRate{sampleRate}
        {
            alloc_all();
        }

        TBufferManager& operator=(const TBufferManager& other) = delete;

        TBufferManager& operator=(TBufferManager&& other)
        {
            free_all();

            // Set input
            fInputBuffer = other.fInputBuffer;
            fOutputBuffer = other.fOutputBuffer;

            fInput = other.fInput;
            fOutput = other.fOutput;

            fBufferSize = other.fBufferSize;
            fSampleRate = other.fSampleRate;

            // Clear output
            other.fInputBuffer = nullptr;
            other.fOutputBuffer = nullptr;

            other.fInput = 0;
            other.fOutput = 0;

            other.fBufferSize = 0;
            other.fSampleRate = 0;

            return *this;
        }

        ~TBufferManager()
        {
        }

    private:
        void alloc(float** ptr, int n_chan)
        {
            if(n_chan > 0)
            {
                ptr[0] = new float[n_chan * fBufferSize];
                for(int i = 1; i < n_chan; i++)
                {
                    ptr[i] = ptr[0] + i * fBufferSize;
                }
            }
        }

        void alloc_all()
        {
            alloc(fInputBuffer, fInput);
            alloc(fOutputBuffer, fOutput);

            UAudioTools::ZeroFloatBlk(fOutputBuffer, fBufferSize, fOutput);
        }

        void free_all()
        {
            if(fInputBuffer)
            {
                delete [] fInputBuffer[0];
            }

            if(fOutputBuffer)
            {
                delete [] fOutputBuffer[0];
            }

            delete [] fInputBuffer;
            delete [] fOutputBuffer;
            fInputBuffer = nullptr;
            fOutputBuffer = nullptr;
        }
};

// We save the original commands in order to reset them when in a loop.
class TGroupAudioMixer : public TExpAudioMixer
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

        void Reset()
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

    private:
        void AddStreamCommand(TCommandPtr command) override
        {
            TExpAudioMixer::AddStreamCommand(command);
            fSavedStreamCommands.insert({command, command->GetDate(), LONG_MAX});
        }
        void StopStreamCommand(TStreamCommandPtr command, SymbolicDate date) override
        {
            TExpAudioMixer::StopStreamCommand(command, date);

            auto it = fSavedStreamCommands.find(command);
            assert(it != fSavedStreamCommands.end());
            it->fStop = date->GetDate();
        }
        void RemoveStreamCommand(TCommandPtr command) override
        {
            TExpAudioMixer::RemoveStreamCommand(command);
            fSavedStreamCommands.erase(fSavedStreamCommands.find(command));
        }


        void AddControlCommand(TCommandPtr command) override
        {
            TExpAudioMixer::AddControlCommand(command);
            fSavedControlCommands.insert({command, command->GetDate(), LONG_MAX});
        }
        void RemoveControlCommand(TCommandPtr command) override
        {
            TExpAudioMixer::RemoveControlCommand(command);
            fSavedControlCommands.erase(fSavedStreamCommands.find(command));
        }
};

class TGroupRenderer :
        public TAudioRenderer
{
    private:
        TBufferManager fBuffers;
        RendererInfo fInfo;

        long OpenImp(
                long inputDevice,
                long outputDevice,
                long inChan,
                long outChan,
                long bufferSize,
                long sampleRate) override;

    public:
        TGroupRenderer();

        virtual ~TGroupRenderer();

        long Open(
                long inChan,
                long outChan,
                long bufferSize,
                long sampleRate) override;

        long Close() override;

        long Start() override;

        long Stop() override;

        long Pause() override;

        long Cont() override;

        void Process(int64_t frames);

        float ** GetOutputBuffer() const;

        void GetInfo(RendererInfoPtr info) override;
};

class TSinusAudioStream final : public TAudioStream
{
        long fDuration{};
        float fFreq{};
        int fCurI = 0;

    public:
        TSinusAudioStream(long duration, float freq) ;

        ~TSinusAudioStream() ;

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override;

        void Reset() override;

        // Cut the beginning of the stream
        TAudioStreamPtr CutBegin(long frames) override;

        // Length in frames
        long Length() override;

        long Channels() override;

        TAudioStreamPtr Copy() override;
};



class TPlayerAudioStream final : public TAudioStream
{
        TGroupRenderer& fRenderer;
        TGroupAudioMixer& fMixer;
        std::atomic_bool fScheduleReset{false};

    public:
        TPlayerAudioStream(
                TGroupRenderer&,
                TGroupAudioMixer&) ;

        ~TPlayerAudioStream() ;

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override;

        void Reset() override;

        // Cut the beginning of the stream
        TAudioStreamPtr CutBegin(long frames) override;

        // Length in frames
        long Length() override;

        long Channels() override;

        TAudioStreamPtr Copy() override;
};

/**
 * @brief The TSendAudioStream class
 *
 * Calls Process on the watched stream and make an inner copy.
 */
class TSendAudioStream final :
        public TDecoratedAudioStream
{
        TBufferManager fBuffers;
        TSharedNonInterleavedAudioBuffer<float> fTempBuffer;
        int fCount = 0;

    public:
        TSendAudioStream(TAudioStreamPtr as):
            TDecoratedAudioStream{as},
            fBuffers{0, as->Channels(), TAudioGlobals::fBufferSize, TAudioGlobals::fSampleRate},
            fTempBuffer{fBuffers.fOutputBuffer, fBuffers.fBufferSize, fBuffers.fOutput}
        {
        }

        ~TSendAudioStream()
        {

        }

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override
        {
            // The process of the send should be transparent.
            // This way, the "send" can be re-used as a stream.
            // If there is nothing more to pull, the result is filled with silence.

            // Clear the local buffer
            UAudioTools::ZeroFloatBlk(fBuffers.fOutputBuffer, fBuffers.fBufferSize, fBuffers.fOutput);

            // Write the new data into the local buffer
            fStream->Read(&fTempBuffer, framesNum, framePos);

            // Mix the local buffer into the output buffer
            float** temp1 = (float**)alloca(buffer->GetChannels()*sizeof(float*));
            UAudioTools::MixFrameToFrameBlk1(buffer->GetFrame(framePos, temp1),
                                             fBuffers.fOutputBuffer,
                                             framesNum,
                                             fBuffers.fOutput);

            fCount++;
            return framesNum;
        }

        float ** GetOutputBuffer() const
        {
            return fBuffers.fOutputBuffer;
        }

        int GetReadBufferCount() const
        {
            return fCount;
        }

        void Reset() override
        {
            fCount = 0;
            TDecoratedAudioStream::Reset();
        }

        TAudioStreamPtr Copy() override
        {
            return new TSendAudioStream{fStream->Copy()};
        }
};
using TSendAudioStreamPtr = LA_SMARTP<TSendAudioStream>;

/**
 * @brief The TReturnAudioStream class
 *
 * Make a copy of the watched stream, and apply a copy on it
 */
class TReturnAudioStream final :
        public TAudioStream
{
        TSendAudioStreamPtr fSend;
        int fLastSendCount = -1;
        bool fStarted = false;
    public:
        TReturnAudioStream(TSendAudioStreamPtr as):
            fSend{as}
        {
            assert(fSend.getPointer());
        }

        ~TReturnAudioStream()
        {

        }

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override
        {
            if(framesNum == 0)
                return 0;

            auto sc = fSend->GetReadBufferCount();
            if(sc <= fLastSendCount)
            {
                // The return has already read its last buffer and was stopped;
                // hence we don't have anything to mix in.
                // Note : if we have some effects generating sound
                // we should instead play a silent buffer. But this applies everywhere.
                return 0;
            }
            else
            {
                fLastSendCount = sc;
            }

            // Write the current buffer to the output.
            const auto channels = buffer->GetChannels();
            const auto channel_bytes = channels * sizeof(float*);

            float** temp1 = (float**) alloca(channel_bytes);

            float ** in_buffer;
            if(framesNum < TAudioGlobals::fBufferSize)
            {
                if(!fStarted)
                {
                    std::cerr << (void*)this << " ==> 0: ";
                    // We haven't started streaming, hence we're mid-buffer.
                    fStarted = true;

                    in_buffer = (float**) alloca(channel_bytes);
                    for(int chan = 0; chan < buffer->GetChannels(); chan++)
                    {
                        in_buffer[chan] = fSend->GetOutputBuffer()[chan] + TAudioGlobals::fBufferSize - framesNum;
                    }
                }
                else
                {
                    std::cerr << (void*)this << " ==> 1: ";
                    // We already started streaming so this is the last buffer
                    // which begins at the frame 0.
                    in_buffer = fSend->GetOutputBuffer();
                }
            }
            else
            {
                //std::cerr << (void*)this << " ==> 2: ";
                in_buffer = fSend->GetOutputBuffer();
            }

            if(fSend->Channels() == buffer->GetChannels())
            {
                UAudioTools::MixFrameToFrameBlk1(buffer->GetFrame(framePos, temp1),
                                                 in_buffer,
                                                 framesNum, fSend->Channels());
            }
            else if(fSend->Channels() < buffer->GetChannels())
            {
                std::cerr << "Not enough channels\n";
                // TODO
                // Two possibilities for channel management :
                // - ableton-like : everything is stereo
                // - S1-like : can create any kind of channels
                // Here we could duplicate, or fill with silence.
            }
            else if(fSend->Channels() > buffer->GetChannels())
            {
                std::cerr << "Too much channels\n";
                // TODO
                // here we should cut the other channels ?
            }
            // Everything should be adapted to work correctly on the "end channels".

            return framesNum;
        }
#include <iscore/tools/Todo.hpp>
        void Reset() override
        {
            ISCORE_TODO;
        }

        // Length in frames
        long Length() override
        {
            return LONG_MAX;
        }

        long Channels() override
        {
            return fSend->Channels();
        }

        TAudioStreamPtr Copy() override
        {
            return new TReturnAudioStream{fSend};
        }
};


/**
 * @brief The TChannelAudioStream class
 *
 * Mixer channel as an audiostream ; for now only volume
 */
class TChannelAudioStream final :
        public TDecoratedAudioStream
{
        double const * const fVolume{};
        TLocalNonInterleavedAudioBuffer<float> fBuffer;
    public:
        TChannelAudioStream(
                TAudioStreamPtr as,
                double const * volume):
            TDecoratedAudioStream{as},
            fVolume{volume},
            fBuffer{TAudioGlobals::fBufferSize, as->Channels()}
        {
        }

        ~TChannelAudioStream()
        {

        }

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override
        {
            float** in = (float**)alloca(fBuffer.GetChannels()*sizeof(float*));
            float** out = (float**)alloca(buffer->GetChannels()*sizeof(float*));
            fBuffer.GetFrame(0, in);
            buffer->GetFrame(framePos, out);

            UAudioTools::ZeroFloatBlk(in, fBuffer.GetSize(), fBuffer.GetChannels());

            long res = fStream->Read(&fBuffer, framesNum, 0);

            auto& vol = *fVolume;
            for(int chan = 0; chan < fBuffer.GetChannels(); chan++)
            {
                for(int j = 0; j < res; j++)
                {
                    out[chan][j] += in[chan][j] * vol;
                }
            }

            return res;
        }

        TAudioStreamPtr Copy() override
        {
            return new TChannelAudioStream{fStream->Copy(), fVolume};
        }
};
using TChannelAudioStreamPtr = LA_SMARTP<TChannelAudioStream>;





#include <Editor/TimeConstraint.h>
#include <Editor/TimeNode.h>
class ExecutorAudioEffect : public TAudioEffectInterface
{
    public:
        ExecutorAudioEffect(OSSIA::TimeConstraint& cst):
            m_root{cst}
        {

        }

    private:

        OSSIA::TimeConstraint& m_root;
        void Process(float** input, float** output, long framesNum) override
        {
            m_root.tick();
            for(int i = 0; i < Channels(); i++)
            {
                std::copy_n(input[i], framesNum, output[i]);
            }
        }

        TAudioEffectInterface* Copy() override
        {
            return nullptr;
        }

        void Reset() override
        {

        }

        long Inputs() override { return Channels(); }
        long Outputs() override { return Channels(); }
        long Channels()
        {
            return 2;
        }

        long GetControlCount() override { return {}; }
        void GetControlParam(long param, char* label, float* min, float* max, float* init) override { }
        void SetControlValue(long param, float value) override { }
        void SetControlValue(const char* label, float value) override { }
        float GetControlValue(long param) override { return {}; }
        float GetControlValue(const char* labe) override { return {}; }

        void SetName(const std::string& name) override { }
        std::string GetName() override { return {}; }

};

//#include <Audio/AudioStreamEngine/Scenario/TimeNodeComponent.hpp>


/*
class LV2AudioEffect : public TAudioEffectInterface
{
    public:
        LV2AudioEffect()
        {

        }

    private:

        OSSIA::TimeConstraint& m_root;
        void Process(float** input, float** output, long framesNum) override
        {
            m_root.tick();
            for(int i = 0; i < Channels(); i++)
            {
                std::copy_n(input[i], framesNum, output[i]);
            }
        }

        TAudioEffectInterface* Copy() override
        {
            return nullptr;
        }

        void Reset() override
        {

        }

        long Inputs() override { return Channels(); }
        long Outputs() override { return Channels(); }
        long Channels()
        {
            return 2;
        }

        long GetControlCount() override { return {}; }
        void GetControlParam(long param, char* label, float* min, float* max, float* init) override { }
        void SetControlValue(long param, float value) override { }
        void SetControlValue(const char* label, float value) override { }
        float GetControlValue(long param) override { return {}; }
        float GetControlValue(const char* labe) override { return {}; }

        void SetName(const std::string& name) override { }
        std::string GetName() override { return {}; }

};*/
/**
 * @brief The TTimeNodeControlCommand struct
 * Only useful for interactive time nodes.
 * Maybe for events too ?
 */
//struct TTimeNodeControlCommand : public TControlCommand
//{
//        TTimeNodeControlCommand(const Audio::AudioStreamEngine::TimeNodeComponent& ctx):
//            m_audio_parent{ctx}
//        {

//        }

//        const Audio::AudioStreamEngine::TimeNodeComponent& m_audio_parent;

//        //std::vector<SymbolicDate> m_beforeDates;
//        //std::vector<SymbolicDate> m_afterDates;

//        bool Execute(
//                TNonInterleavedAudioBuffer<float>* buffer,
//                map<SymbolicDate, audio_frame_t>& date_map,
//                audio_frame_t cur_frame,
//                long frames)
//        {
//            auto timenode_time = fStartDate->getDate();
//            if (InBuffer(timenode_time, cur_frame, frames))
//            {
//                m_audio_parent.onDateFixed(cur_frame);

//                // Set stop date of all previous constraint to the TN frame
//                for(SymbolicDate date : m_beforeDates)
//                    date->setDate(timenode_time);

//                // Execute the time node.
//                // If we are here, it has already been triggered.
//                // We only have to check the conditions.
//                m_node.trigger();

//                // For all true events, set start date of all next constraints's start, and
//                // trigger their start / their start + offset.
//                for(SymbolicDate date : m_afterDates)
//                    date->setDate(timenode_time + 1);


//                // TODO after each command, the commands should be re-sorted.
//                // TODO the commands should have a priority for hierarchy ?

//                // TODO else find a way to disable' em

//                return false;
//            }
//            else
//            {
//                return true;
//            }
//        }

//};
