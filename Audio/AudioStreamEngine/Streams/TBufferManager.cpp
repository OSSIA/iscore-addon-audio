#include "TBufferManager.hpp"
#include <UAudioTools.h>

TBufferManager::TBufferManager(long inChan, long outChan, long bufferSize, long sampleRate):
    fInputBuffer{inChan > 0 ? new float*[inChan] : nullptr},
    fOutputBuffer{outChan > 0 ? new float*[outChan] : nullptr},
    fInput{inChan},
    fOutput{outChan},
    fBufferSize{bufferSize},
    fSampleRate{sampleRate}
{
    alloc_all();
}

TBufferManager &TBufferManager::operator=(TBufferManager&& other)
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

TBufferManager::~TBufferManager()
{
    free_all();
}

void TBufferManager::alloc(float **ptr, int n_chan)
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

void TBufferManager::alloc_all()
{
    alloc(fInputBuffer, fInput);
    alloc(fOutputBuffer, fOutput);

    UAudioTools::ZeroFloatBlk(fOutputBuffer, fBufferSize, fOutput);
}

void TBufferManager::free_all()
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
