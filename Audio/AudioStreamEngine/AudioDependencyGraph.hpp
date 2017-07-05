#pragma once
#include <Audio/AudioStreamEngine/AudioGraphNode.hpp>
#include <QByteArray>
namespace iscore
{
struct DocumentContext;
}
namespace Audio
{
namespace AudioStreamEngine
{
struct Context;
class Constraint;

struct AudioGraphBuilder
{
        AudioGraph m_graph;

    public:
        AudioGraphBuilder(const iscore::DocumentContext& ctx, Constraint& root);

        boost::optional<std::deque<int>> check() const;
        void apply(const std::deque<int>&, Context& ctx);

        std::string toGraphViz();
};

}
}
