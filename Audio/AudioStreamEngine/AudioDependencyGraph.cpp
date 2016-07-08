#include "AudioDependencyGraph.hpp"


#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Audio/MixProcess/MixProcessModel.hpp>
#include <Audio/SendProcess/SendProcessModel.hpp>
#include <Audio/ReturnProcess/ReturnProcessModel.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>

#include <Audio/AudioStreamEngine/Audio/EffectComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/SoundComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/SendComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/ReturnComponent.hpp>

#include <Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/ScenarioComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/LoopComponent.hpp>
namespace Audio
{
namespace AudioStreamEngine
{

namespace {
struct get_id {
        Id<Process::ProcessModel> operator()(ConstraintComponent* comp)
        {
            return Id<Process::ProcessModel>{};
        }

        template<typename T>
        Id<Process::ProcessModel> operator()(T* component)
        {
            return component->process().id();
        }
};
}

AudioDependencyGraph::AudioDependencyGraph(ConstraintComponent &root)
{
    // 1. Create vertices
    visit(root);

    // 2. For all "Return" vertices, add edge to "Send"
    // since the return depends on the send
    std::vector<std::string> names;
    auto vertices = boost::vertices(m_graph);
    for(auto it = vertices.first; it != vertices.second; ++it)
    {
        auto asComp = static_cast<QObject**>(m_graph[*it].target());
        if(asComp)
            names.push_back((*asComp)->objectName().toStdString());
        // For all the returns
        if(auto return_vertice = m_graph[*it].target<ReturnComponent*>())
        {
            auto send = (*return_vertice)->process().send_ptr();
            if(!send)
                continue;

            auto& send_comp = iscore::component<SendComponent>(send->components);

            // Then find the corresponding send
            for(auto it_k = vertices.first; it_k != vertices.second; ++it_k)
            {
                if(m_graph[*it_k] == &send_comp)
                {
                    // Add an edge from return to send
                    boost::add_edge(*it_k, *it, m_graph);
                    break;
                }
            }
        }
    }

    // 3. Generate image of the graph.
    boost::write_graphviz(std::cerr, m_graph,
                          [&] (std::ostream& out, const auto& v) {
        out << "[label=\"" << names[v] << "\"]";
    },
    [] (auto&&...) {});
}

boost::optional<std::deque<int> > AudioDependencyGraph::check() const
{
    // Do a topological sort
    try {
        std::deque<int> topo_order;
        boost::topological_sort(m_graph, std::front_inserter(topo_order));


        /*
                qDebug() << "SIZE/" << topo_order.size();
                // Print the results.
                for(auto elt : topo_order)
                {
                    struct
                    {
                            void operator()(const IdentifiedObjectAbstract* ptr)
                            {
                                qDebug() << ptr->objectName() << " " << ptr->id_val();
                            }
                    } s;
                    eggs::variants::apply(s, m_graph[elt]);
                }
                */

        // For each element in the queue, create the stream.
        // It has the insurance that the required streams already exist.

        return topo_order;
    }
    catch(const boost::not_a_dag& e)
    {
        qDebug() << e.what();
        return boost::none;
    }

    return boost::none;
}


struct AudioGraphVisitor
{
        Context& ctx;

        template<typename T>
        void operator()(T* component)
        {
            component->makeStream(ctx);
            if(!component->getStream())
            {
                qDebug() << "Warning: component does not have a stream";
            }
        }
};

void AudioDependencyGraph::apply(const std::deque<int>& sorted_vertices, Context& ctx)
{
    AudioGraphVisitor v{ctx};
    for(auto vertice : sorted_vertices)
    {
        eggs::variants::apply(v, m_graph[vertice]);
    }
}

AudioDependencyGraph::vtx_t AudioDependencyGraph::visit(ConstraintComponent &cst)
{
    auto res = boost::add_vertex(&cst, m_graph);

    int n_proc = cst.processes().size();
    std::vector<vtx_t> processes;
    processes.reserve(n_proc);
    std::vector<vtx_t> generators; // Everything that output a mixable stream
    generators.reserve(n_proc);
    std::vector<vtx_t> inputs; // FX, Send
    inputs.reserve(n_proc / 2);

    // If there is no mix process, we assume that everything is
    // routed into everything.
    // Else, we use it to create the correct relations.
    // TODO think of the policies !!!!
    for(auto& proc_pair : cst.processes())
    {
        auto& proc = proc_pair.component;
        if(auto scenar = dynamic_cast<ScenarioComponent*>(&proc))
        {
            auto sub_res = visit(*scenar);
            generators.push_back(sub_res);
            processes.push_back(sub_res);
        }
        else if(auto loop = dynamic_cast<LoopComponent*>(&proc))
        {
            auto sub_res = visit(*loop);
            generators.push_back(sub_res);
            processes.push_back(sub_res);
        }
        else if(auto fx = dynamic_cast<EffectProcessComponent*>(&proc))
        {
            auto sub_res = visit(*fx);
            generators.push_back(sub_res);
            inputs.push_back(sub_res);
            processes.push_back(sub_res);
        }
        else if(auto send = dynamic_cast<SendComponent*>(&proc))
        {
            auto sub_res = visit(*send);
            inputs.push_back(sub_res);
            processes.push_back(sub_res);
        }
        else if(auto ret = dynamic_cast<ReturnComponent*>(&proc))
        {
            auto send = ret->process().send_ptr();
            if(send)
            {
                auto sub_res = visit(*ret);
                generators.push_back(sub_res);
                processes.push_back(sub_res);
            }
        }
        else if(auto sound = dynamic_cast<SoundComponent*>(&proc))
        {
            auto sub_res = visit(*sound);
            generators.push_back(sub_res);
            processes.push_back(sub_res);
        }
    }

    // For all inputs, generate edges going from all generators

    const auto mix = cst.findMix();
    for(auto input : inputs)
    {
        for(auto gen : generators)
        {
            if(gen != input)
            {
                if(mix)
                {
                    auto gen_id = eggs::variants::apply(get_id{}, m_graph[gen]);
                    ISCORE_ASSERT(bool(gen_id));
                    auto in_id = eggs::variants::apply(get_id{}, m_graph[input]);
                    ISCORE_ASSERT(bool(in_id));

                    // TODO transform this into an assert when the FX's can be routed each into
                    // each other.
                    auto routing_it = mix->routings().find(Mix::Routing{gen_id, in_id});
                    if(routing_it == mix->routings().end())
                    {
                        qDebug() << "routing not found in mix";
                    }
                    else if(routing_it->enabled)
                    {
                        boost::add_edge(gen, input, m_graph);
                    }
                }
                else
                {
                    boost::add_edge(gen, input, m_graph);
                }
            }
        }
    }

    // Add edge from all processes to the constraint mix stream
    // Even for the sends : the constraint "pulls" the send process
    // to ensure that it is up-to-date.
    for(auto proc : processes)
    {
        boost::add_edge(proc, res, m_graph);

    }
    return res;
}

AudioDependencyGraph::vtx_t AudioDependencyGraph::visit(ScenarioComponent &proc)
{
    // Create a node for the group and nodes for
    // all the constraints
    auto res = boost::add_vertex(&proc, m_graph);

    for(auto& constraint : proc.constraints())
    {
        auto cst_vtx = visit(constraint.component);

        boost::add_edge(cst_vtx, res, m_graph);
    }

    return res;
}

AudioDependencyGraph::vtx_t AudioDependencyGraph::visit(EffectProcessComponent &proc)
{
    // Create a return and a send
    return boost::add_vertex(&proc, m_graph);

}

AudioDependencyGraph::vtx_t AudioDependencyGraph::visit(LoopComponent &proc)
{
    auto res = boost::add_vertex(&proc, m_graph);

    // Create a node for the group player
    for(auto& constraint : proc.constraints())
    {
        auto cst_vtx = visit(constraint.component);

        boost::add_edge(cst_vtx, res, m_graph);
    }

    return res;
}

AudioDependencyGraph::vtx_t AudioDependencyGraph::visit(SendComponent &proc)
{
    // Create an input and an output node
    return boost::add_vertex(&proc, m_graph);
}

AudioDependencyGraph::vtx_t AudioDependencyGraph::visit(ReturnComponent &proc)
{
    // Create an output node
    return boost::add_vertex(&proc, m_graph);
}

AudioDependencyGraph::vtx_t AudioDependencyGraph::visit(SoundComponent &proc)
{
    // Only create a send
    return boost::add_vertex(&proc, m_graph);
}


}
}
