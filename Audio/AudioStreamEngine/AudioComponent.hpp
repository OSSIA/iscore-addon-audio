#pragma once
#include <iscore/component/Component.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

class Component : public iscore::GenericComponent<DocumentPlugin>
{
    public:
        using iscore::GenericComponent<DocumentPlugin>::GenericComponent;
        virtual ~Component();
        virtual AudioGraphVertice visit(AudioGraph& graph) = 0;
        virtual void makeStream(const Context& player) = 0;

        AudioStream getStream() const { return m_stream; }

    protected:
        AudioStream m_stream{};
};

}
}
