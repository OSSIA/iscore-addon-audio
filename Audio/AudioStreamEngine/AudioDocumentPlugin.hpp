#pragma once
#include <iscore/plugins/documentdelegate/plugin/DocumentDelegatePluginModel.hpp>
#include <3rdparty/libaudiostream/src/LibAudioStreamMC++.h>
#include <core/document/Document.hpp>
#include <Process/TimeValue.hpp>
namespace Audio
{
namespace AudioStreamEngine
{
struct Context
{
        const iscore::DocumentContext& doc;
        AudioPlayerPtr player{};
        AudioRendererPtr renderer{};
        DeviceInfo device_info{};
        RendererInfo renderer_info{};

        long time(const TimeValue& t)
        {
            return t.msec() * time_factor;
        }

        const double time_factor{};
};

class DocumentPlugin : public iscore::DocumentPlugin
{
    public:
        DocumentPlugin(
                iscore::Document& doc,
                QObject* parent):
            iscore::DocumentPlugin{doc, "AudioDocumentPlugin", parent},
            m_ctx{doc.context()}
        {

        }

        void play();
        void stop();

    private:
        Context m_ctx;
};
}
}
