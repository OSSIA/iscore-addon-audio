#pragma once
#include <renderer/TAudioRenderer.h>
#include "TBufferManager.hpp"

class TGroupRenderer final :
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
