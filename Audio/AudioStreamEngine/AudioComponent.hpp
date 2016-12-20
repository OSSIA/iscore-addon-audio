#pragma once
#include <iscore/model/Component.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>

#include <ossia/editor/state/state_element.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

class Component : public iscore::GenericComponent<DocumentPlugin>
{
    public:
        using iscore::GenericComponent<DocumentPlugin>::GenericComponent;
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
