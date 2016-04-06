#include "AudioEngine.hpp"
#include <iscore/tools/std/Algorithms.hpp>

AudioEngine::AudioEngine()
{
    m_handles.reserve(100);
}

AudioEngine::~AudioEngine()
{
    std::lock_guard<std::mutex> lock(handles_lock);
    for(auto blck : m_handles)
    {
        blck->m_deleting = true;
    }
    m_handles.clear();
}

void AudioEngine::play()
{
    output.startStream([&] (void* outputBuffer, int size) {

        float *out = static_cast<float *>(outputBuffer);
        for(std::size_t i = 0; i < 2 * size; i++)
            out[i] = 0;

        bool turn = true;
        std::size_t handle_i = 0;
        while(turn)
        {
            std::vector<AudioBlock*> handles;
            {
                std::lock_guard<std::mutex> lock(handles_lock);

                if(m_handles.size() == 0)
                    return 0;

                handles = this->m_handles;
            }

            // Mark all "played" handles to remove them and only loop on unplayed handles.
            if(handle_i < handles.size())
            {
                AudioBlock* proc = handles[handle_i];
                auto current = proc->data(size, proc->currentBuffer++, 0);

                switch(current.size())
                {
                    case 0: // nothing
                        break;
                    case 1:
                    {
                        auto& chan = current[0];
                        if(chan.size() >= size)
                        {
                            // Duplicate on each channel
                            for(std::size_t i = 0; i < size; i++)
                            {
                                out[i] += chan[i] * 0.2;
                            }

                            float* out_2 = out + size;

                            for(std::size_t i = 0; i < size; i++)
                            {
                                out_2[i] += chan[i] * 0.2;
                            }
                        }
                        break;
                    }
                    case 2:
                    {
                        for(int i_chan = 0 ; i_chan < 2; i_chan++)
                        {
                            auto& chan = current[i_chan];
                            if(chan.size() >= size)
                            {
                                // Duplicate on each channel
                                float* current_out = out + i_chan * size;
                                for(std::size_t i = 0; i < size; i++)
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

    std::lock_guard<std::mutex> lock(handles_lock);
    for(auto blck : m_handles)
    {
        blck->currentBuffer = 0;
        blck->offset = 0;
    }
    m_handles.clear();
}

void AudioEngine::addHandle(AudioBlock* block)
{
    std::lock_guard<std::mutex> lock(handles_lock);

    auto it = find(m_handles, block);
    if(it == m_handles.end())
        m_handles.push_back(block);
}

void AudioEngine::removeHandle(AudioBlock* block)
{
    std::lock_guard<std::mutex> lock(handles_lock);
    auto it = find(m_handles, block);
    if(it != m_handles.end())
    {
        AudioBlock* blck = *it;
        blck->currentBuffer = 0;
        blck->offset = 0;
        m_handles.erase(it);
    }
}
