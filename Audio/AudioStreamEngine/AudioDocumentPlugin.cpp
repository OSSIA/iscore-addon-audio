#include "AudioDocumentPlugin.hpp"
#include <Scenario/Process/ScenarioModel.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <map>
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp>
#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>

#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Audio/MixProcess/MixProcessModel.hpp>
#include <Audio/SendProcess/SendProcessModel.hpp>
#include <Audio/ReturnProcess/ReturnProcessModel.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>

#include <Audio/AudioStreamEngine/Audio/EffectComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/MixComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/SoundComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/SendComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/ReturnComponent.hpp>

#include <Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/ScenarioComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/LoopComponent.hpp>

#include <boost/graph/adjacency_list.hpp>

#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/topological_sort.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

struct AudioDependencyGraph
{
        using node_t = eggs::variant<
            ConstraintComponent*,
            ScenarioComponent*,
            EffectComponent*,
            LoopComponent*,
            SendComponent*,
            ReturnComponent*,
            SoundComponent*
        >;
        boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, node_t> m_graph;

        using vtx_t = decltype(boost::add_vertex(node_t{}, m_graph));

    public:
        SendComponent* getComponent(const Send::ProcessModel& proc)
        {
            // TODO add an UUID index ?
            for(auto& comp : proc.components)
            {
                if(auto res = dynamic_cast<SendComponent*>(&comp))
                {
                    return res;
                }
            }

            return nullptr;
        }

        AudioDependencyGraph(ConstraintComponent& root)
        {
            // 1. Create vertices
            visit(root);

            // 2. For all "Return" vertices, add edge to "Send"
            // since the return depends on the send
            auto vertices = boost::vertices(m_graph);
            for(auto it = vertices.first; it != vertices.second; ++it)
            {
                // For all the returns
                if(auto return_vertice = m_graph[*it].target<const ReturnComponent*>())
                {
                    auto send = (*return_vertice)->process().send_ptr();
                    if(!send)
                        continue;

                    auto send_comp = getComponent(*send);
                    ISCORE_ASSERT(send_comp);

                    // Then find the corresponding send
                    for(auto it_k = vertices.first; it_k != vertices.second; ++it_k)
                    {
                        if(m_graph[*it_k] == send_comp)
                        {
                            // Add an edge from return to send
                            boost::add_edge(*it, *it_k, m_graph);
                            break;
                        }
                    }
                }
            }

            // 3. Generate image of the graph.
            //boost::write_graphviz(std::cout, m_graph);
        }

        boost::optional<std::deque<int>> check() const
        {
            // Do a topological sort
            try {
                std::deque<int> topo_order;
                boost::topological_sort(m_graph, std::front_inserter(topo_order));


                qDebug() << "SIZE/" << topo_order.size();
                // Print the results.
                for(auto elt : topo_order)
                {
                    struct
                    {
                            void operator()(const QObject* ptr)
                            {
                                qDebug() << ptr->objectName();
                            }
                    } s;
                    eggs::variants::apply(s, m_graph[elt]);
                }


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

    private:
        vtx_t visit(ConstraintComponent& cst)
        {
            auto res = boost::add_vertex(&cst, m_graph);

            int n_proc = cst.processes().size();
            std::vector<vtx_t> processes;
            processes.reserve(n_proc);
            std::vector<vtx_t> generators; // Everything that output a mixable stream
            generators.reserve(n_proc);
            std::vector<vtx_t> inputs; // FX, Send
            inputs.reserve(n_proc / 2);

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
                else if(auto fx = dynamic_cast<EffectComponent*>(&proc))
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
                else if(auto mix = dynamic_cast<MixComponent*>(&proc))
                {
                    //visit(*mix);

                }
            }

            // For all inputs, generate edges going from all generators
            for(auto input : inputs)
            {
                for(auto gen : generators)
                {
                    if(gen != input)
                    {
                        boost::add_edge(gen, input, m_graph);
                    }
                }
            }

            // Add edge from all processes to the constraint mix stream
            for(auto proc : processes)
            {
                boost::add_edge(proc, res, m_graph);

            }
            return res;
        }

        vtx_t visit(ScenarioComponent& proc)
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

        vtx_t visit(EffectComponent& proc)
        {
            // Create a return and a send
            return boost::add_vertex(&proc, m_graph);

        }

        vtx_t visit(LoopComponent& proc)
        {
            auto res = boost::add_vertex(&proc, m_graph);

            // Create a node for the group player

            return res;
        }

        vtx_t visit(SendComponent& proc)
        {
            // Create an input and an output node
            return boost::add_vertex(&proc, m_graph);
        }

        vtx_t visit(ReturnComponent& proc)
        {
            // Create an output node
            return boost::add_vertex(&proc, m_graph);
        }

        vtx_t visit(SoundComponent& proc)
        {
            // Only create a send
            return boost::add_vertex(&proc, m_graph);
        }


    private:
};

struct AudioGraphVisitor
{
        Context& ctx;

        template<typename T>
        void operator()(T* component)
        {
            component->makeStream(ctx);
        }
};

void DocumentPlugin::play()
{
    // First find the root constraint
    auto doc = dynamic_cast<Scenario::ScenarioDocumentModel*>(&m_ctx.doc.document.model().modelDelegate());
    if(!doc)
        return;

    qDebug() << (void*)m_ctx.audio.renderer ;
    if(!m_ctx.audio.renderer)
    {
        iscore::AppContext().components.applicationPlugin<Audio::AudioStreamEngine::ApplicationPlugin>().startEngine();
    }

    if(!m_ctx.audio.plugin.engineStatus())
        return;

    // Reset the player
    stop();
    openPlayer();


    // Create our tree
    auto comp = new ConstraintComponent(
                Id<iscore::Component>{1},
                doc->baseConstraint(),
                *this,
                m_ctx.doc,
                this);
    doc->baseConstraint().components.add(comp);
    AudioDependencyGraph graph{*comp};
    if(auto sorted_vertices = graph.check())
    {
        AudioGraphVisitor v{m_ctx};
        for(auto vertice : *sorted_vertices)
        {
            eggs::variants::apply(v, graph.m_graph[vertice]);
        }

    }
    else
    {
        stop();
        return;
    }

    con(m_ctx.doc.document, &iscore::Document::aboutToClose,
        this, [=] () {
        // Stop
        stop();

        // Delete
        doc->baseConstraint().components.remove(Id<iscore::Component>{1});
    });


    // First we have to construct our graph

    // If the graph is acyclic, we can walk it and toposort it to
    // create the streams
    // finnally, we can associate the streams with their elements.

    // Play

    // TODO make id from components !!!!
    if(comp)
    {
        m_stream = comp->getStream();
    }
    else
    {
        qDebug("No component!");
    }

    if(m_stream)
    {
        StartSound(m_ctx.audio.player, m_stream, GenRealDate(m_ctx.audio.player, 0));
    }
    else
    {
        qDebug("No stream!");
    }

    StartAudioPlayer(m_ctx.audio.player);
}

void DocumentPlugin::stop()
{
    if(m_ctx.audio.player)
    {
        StopAudioPlayer(m_ctx.audio.player);
        CloseAudioClient(m_ctx.audio.player);
        m_ctx.audio.player = nullptr;
    }

    auto doc = dynamic_cast<Scenario::ScenarioDocumentModel*>(&m_ctx.doc.document.model().modelDelegate());
    if(doc)
    {
        auto it = doc->baseConstraint().components.find(Id<iscore::Component>{1});
        if(it != doc->baseConstraint().components.end())
        {
            doc->baseConstraint().components.remove(Id<iscore::Component>{1});
        }
    }

    m_stream = {};
}

void DocumentPlugin::openPlayer()
{
    if(!m_ctx.audio.player)
    {
        m_ctx.audio.player = OpenAudioClient(m_ctx.audio.renderer);
    }
}

}
}
