#pragma once
#include <Audio/AudioStreamEngine/AudioGraphNode.hpp>
#include <QByteArray>
namespace score
{
struct DocumentContext;
}
namespace Audio
{
namespace AudioStreamEngine
{
struct Context;
class Interval;

struct AudioGraphBuilder
{
        AudioGraph m_graph;

    public:
        AudioGraphBuilder(const score::DocumentContext& ctx, Interval& root);

        boost::optional<std::deque<int>> check() const;
        void apply(const std::deque<int>&, Context& ctx);

        std::string toGraphViz();
};

}
}
