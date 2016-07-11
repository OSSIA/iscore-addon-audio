#pragma once
#include <Audio/AudioStreamEngine/AudioGraphNode.hpp>

namespace Audio
{
namespace AudioStreamEngine
{
struct Context;
struct Constraint;

struct AudioGraphBuilder
{
        AudioGraph m_graph;

    public:
        AudioGraphBuilder(Constraint& root);

        boost::optional<std::deque<int>> check() const;
        void apply(const std::deque<int>&, Context& ctx);
};

}
}
