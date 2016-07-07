#pragma once


#include <boost/graph/adjacency_list.hpp>

#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/topological_sort.hpp>

namespace Process
{
class ProcessModel;
}
namespace Audio
{
namespace Send
{
class ProcessModel;
}
namespace Effect
{
class ProcessModel;
}
namespace AudioStreamEngine
{
struct Context;
class ConstraintComponent;
class SendComponent;
class EffectProcessComponent;
class ReturnComponent;
class SoundComponent;
class ScenarioComponent;
class LoopComponent;

struct AudioDependencyGraph
{
        using node_t = eggs::variant<
            ConstraintComponent*,
            ScenarioComponent*,
            EffectProcessComponent*,
            LoopComponent*,
            SendComponent*,
            ReturnComponent*,
            SoundComponent*
        >;

        boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, node_t> m_graph;

        using vtx_t = decltype(boost::add_vertex(node_t{}, m_graph));

    public:
        AudioDependencyGraph(ConstraintComponent& root);

        boost::optional<std::deque<int>> check() const;
        void apply(const std::deque<int>&, Context& ctx);

    private:
        vtx_t visit(ConstraintComponent& cst);
        vtx_t visit(ScenarioComponent& proc);
        vtx_t visit(EffectProcessComponent& proc);
        vtx_t visit(LoopComponent& proc);
        vtx_t visit(SendComponent& proc);
        vtx_t visit(ReturnComponent& proc);
        vtx_t visit(SoundComponent& proc);

    private:
};

}
}
