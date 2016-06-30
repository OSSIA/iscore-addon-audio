#pragma once

/**
 * @brief TBufferManager struct : a buffer where all channels are
 * allocated contiguously
 */
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
                long sampleRate);

        TBufferManager& operator=(const TBufferManager& other) = delete;

        TBufferManager& operator=(TBufferManager&& other);

        ~TBufferManager();

    private:
        void alloc(float** ptr, int n_chan);

        void alloc_all();

        void free_all();
};
