#include "AudioEngine.hpp"
#include <iscore/tools/std/Algorithms.hpp>

void AudioEngine::play()
{
	output.startStream([&] (void* outputBuffer) {
        float *out = static_cast<float *>(outputBuffer);
        for(std::size_t i = 0; i < 2 * params.bufferSize; i++)
            out[i] = 0;

        bool turn = true;
        std::size_t handle_i = 0;
        while(turn)
        {
            std::lock_guard<std::mutex> lock(handles_lock);
            // Mark all "played" handles to remove them and only loop on unplayed handles.
            if(handle_i < handles.size())
            {
                AudioBlock* proc = handles[handle_i];
                auto current = proc->data(params.bufferSize, proc->currentBuffer++, 0);

                switch(current.size())
                {
                    case 0: // nothing
                        break;
                    case 1:
                    {
                        auto& chan = current[0];
                        if(chan.size() >= params.bufferSize)
                        {
                            // Duplicate on each channel
                            for(std::size_t i = 0; i < params.bufferSize; i++)
                            {
                                out[i] += chan[i] * 0.2;
                            }

                            out = out + params.bufferSize;

                            for(std::size_t i = 0; i < params.bufferSize; i++)
                            {
                                out[i] += chan[i] * 0.2;
                            }
                        }
                        break;
                    }
                    case 2:
                    {
                        for(int i_chan = 0 ; i_chan < 2; i_chan++)
                        {
                            auto& chan = current[i_chan];
                            if(chan.size() >= params.bufferSize)
                            {
                                // Duplicate on each channel
                                float* current_out = out + i_chan * params.bufferSize;
                                for(std::size_t i = 0; i < params.bufferSize; i++)
                                {
                                    current_out[i] += chan[i] * 0.2;
                                }
                            }
                        }
                    }
                    default: // not yet
                        break;
                }


                handle_i++;
            }
            else
            {
                turn = false;
            }
        }

        return 0;
    });
}

void AudioEngine::stop()
{
    output.stopStream();

}

void AudioEngine::addHandle(AudioBlock* block)
{
    std::lock_guard<std::mutex> lock(handles_lock);

    auto it = find(handles, block);
    if(it == handles.end())
        handles.push_back(block);
}

void AudioEngine::removeHandle(AudioBlock* block)
{
    std::lock_guard<std::mutex> lock(handles_lock);
    auto it = find(handles, block);
    if(it != handles.end())
    {
        handles.erase(it);
    }
}
