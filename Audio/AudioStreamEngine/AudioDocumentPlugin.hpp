#pragma once
#include <iscore/plugins/documentdelegate/plugin/DocumentDelegatePluginModel.hpp>
#include <core/document/Document.hpp>
#include <Audio/AudioStreamEngine/Context.hpp>
#include <Process/TimeValue.hpp>
#include <Audio/Panel/TrackModel.hpp>

namespace Audio
{
namespace AudioStreamEngine
{
class DocumentPlugin : public iscore::DocumentPlugin
{
    public:
        DocumentPlugin(
                AudioContext& ctx,
                iscore::Document& doc,
                QObject* parent):
            iscore::DocumentPlugin{doc.context(), "AudioDocumentPlugin", parent},
            m_ctx{doc.context(), ctx}, m_trackModel{doc.context(), this}
        {

        }

        void play();
        void stop();
        Panel::TrackModel& trackModel();

    private:
        void openPlayer();

        Context m_ctx;
        AudioStream m_stream;
        Panel::TrackModel m_trackModel;
};
}
}
