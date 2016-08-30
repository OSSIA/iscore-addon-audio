#pragma once
#include <iscore/component/Component.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>

#include <ossia/editor/state/state_element.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

class Component : public iscore::SystemComponent<iscore::Component, DocumentPlugin>
{
    public:
        using iscore::SystemComponent<iscore::Component, DocumentPlugin>::SystemComponent;
        virtual ~Component();
        virtual optional<AudioGraphVertice> visit(AudioGraph& graph) = 0;
        virtual void makeStream(const Context& player) = 0;

        virtual QString prettyName() const { return objectName(); }

        bool realTime() const { return m_realTime; }
        void setRealTime(bool b) { m_realTime = b; }

        AudioStream getStream() const { return m_stream; }

        int priority = 0;
    protected:
        AudioStream m_stream{};
        bool m_realTime{};
};

}
}
