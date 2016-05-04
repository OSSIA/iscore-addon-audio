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

#include <Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/ScenarioComponent.hpp>

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
            const Scenario::ConstraintModel*,
            const Scenario::ScenarioModel*,
            const Effect::ProcessModel*,
            const Loop::ProcessModel*,
            const Send::ProcessModel*,
            const Return::ProcessModel*,
            const Sound::ProcessModel*
        >;
        boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, node_t> m_graph;

        using vtx_t = decltype(boost::add_vertex(node_t{}, m_graph));

    public:
        AudioDependencyGraph(const Scenario::ConstraintModel& root)
        {
            // 1. Create vertices
            visit(root);

            // 2. For all "Return" vertices, add edge to "Send"
            // since the return depends on the send
            auto vertices = boost::vertices(m_graph);
            for(auto it = vertices.first; it != vertices.second; ++it)
            {
                // For all the returns
                if(auto return_vertice = m_graph[*it].target<const Return::ProcessModel*>())
                {
                    auto send = (*return_vertice)->send_ptr();
                    if(!send)
                        continue;

                    // Then find the corresponding send
                    for(auto it_k = vertices.first; it_k != vertices.second; ++it_k)
                    {
                        if(m_graph[*it_k] == send)
                        {
                            // Add an edge from return to send
                            boost::add_edge(*it, *it_k, m_graph);
                            break;
                        }
                    }
                }
            }

            // 3. Generate image of the graph.
            boost::write_graphviz(std::cout, m_graph);
        }

        bool check() const
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

                return true;
            }
            catch(const boost::not_a_dag& e)
            {
                qDebug() << e.what();
                return false;
            }

        }

        auto createStreams()
        {

        }

    private:
        vtx_t visit(const Scenario::ConstraintModel& cst)
        {
            auto res = boost::add_vertex(&cst, m_graph);

            std::vector<vtx_t> processes;
            processes.reserve(cst.processes.size());
            std::vector<vtx_t> generators; // Everything that output a mixable stream
            generators.reserve(cst.processes.size());
            std::vector<vtx_t> inputs; // FX, Send
            inputs.reserve(cst.processes.size() / 2);

            for(auto& proc : cst.processes)
            {
                if(auto scenar = dynamic_cast<const Scenario::ScenarioModel*>(&proc))
                {
                    auto sub_res = visit(*scenar);
                    generators.push_back(sub_res);
                    processes.push_back(sub_res);
                }
                else if(auto loop = dynamic_cast<const Loop::ProcessModel*>(&proc))
                {
                    auto sub_res = visit(*loop);
                    generators.push_back(sub_res);
                    processes.push_back(sub_res);
                }
                else if(auto fx = dynamic_cast<const Effect::ProcessModel*>(&proc))
                {
                    auto sub_res = visit(*fx);
                    generators.push_back(sub_res);
                    inputs.push_back(sub_res);
                    processes.push_back(sub_res);
                }
                else if(auto send = dynamic_cast<const Send::ProcessModel*>(&proc))
                {
                    auto sub_res = visit(*send);
                    inputs.push_back(sub_res);
                    processes.push_back(sub_res);
                }
                else if(auto ret = dynamic_cast<const Return::ProcessModel*>(&proc))
                {
                    auto send = ret->send_ptr();
                    if(send)
                    {
                        auto sub_res = visit(*ret);
                        generators.push_back(sub_res);
                        processes.push_back(sub_res);
                    }
                }
                else if(auto sound = dynamic_cast<const Sound::ProcessModel*>(&proc))
                {
                    auto sub_res = visit(*sound);
                    generators.push_back(sub_res);
                    processes.push_back(sub_res);
                }
                else if(auto mix = dynamic_cast<const Mix::ProcessModel*>(&proc))
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

        vtx_t visit(const Scenario::ScenarioModel& proc)
        {
            // Create a node for the group and nodes for
            // all the constraints
            auto res = boost::add_vertex(&proc, m_graph);

            for(auto& constraint : proc.constraints)
            {
                auto cst_vtx = visit(constraint);

                boost::add_edge(cst_vtx, res, m_graph);
            }

            return res;
        }

        vtx_t visit(const Effect::ProcessModel& proc)
        {
            // Create a return and a send
            return boost::add_vertex(&proc, m_graph);

        }

        vtx_t visit(const Loop::ProcessModel& proc)
        {
            auto res = boost::add_vertex(&proc, m_graph);

            // Create a node for the group player

            return res;
        }

        vtx_t visit(const Send::ProcessModel& proc)
        {
            // Create an input and an output node
            return boost::add_vertex(&proc, m_graph);
        }

        vtx_t visit(const Return::ProcessModel& proc)
        {
            // Create an output node
            return boost::add_vertex(&proc, m_graph);
        }

        vtx_t visit(const Sound::ProcessModel& proc)
        {
            // Only create a send
            return boost::add_vertex(&proc, m_graph);
        }


    private:
};

AudioStream CreateAudioStream(const Loop::ProcessModel& loop);
AudioStream CreateAudioStream(const Loop::ProcessModel& loop)
{
    // Apply to the constraint
    // If trigger at the end, see:
    /*
    date = GetCurDate();
    symb1 = GenSymbolicDate(gAudioPlayer);

    s1 = MakeLoopSound(MakeRegionSound(FILENAME1, 5*SR, 100*SR), INFINI);
    s2 = MakeRegionSound(FILENAME2, 5*SR, 10*SR);

    StartSound(gAudioPlayer, s1, GenRealDate(gAudioPlayer, date));
    StopSound(gAudioPlayer, s1, symb1);
    StartSound(gAudioPlayer, s2, symb1);

    // Instancie la date symbolique à la date courante "capturée"
    set_symbolic_date(symb1);
    */


    return {};
}

void DocumentPlugin::play()
{
    // First find the root constraint
    auto doc = dynamic_cast<Scenario::ScenarioDocumentModel*>(&m_ctx.doc.document.model().modelDelegate());
    if(!doc)
        return;
    AudioDependencyGraph graph{doc->baseConstraint()};
    qDebug() << graph.check();
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

    // TODO make id from components !!!!
    startPlayer();

    // Create our tree
    auto comp = new ConstraintComponent(
                Id<iscore::Component>{1},
                doc->baseConstraint(),
                *this,
                m_ctx.doc,
                this);
    doc->baseConstraint().components.add(comp);

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

    StartAudioPlayer(m_ctx.audio.player);
    if(comp)
    {
        m_stream = comp->makeStream(
                    m_ctx);
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

void DocumentPlugin::startPlayer()
{
    if(!m_ctx.audio.player)
    {
        m_ctx.audio.player = OpenAudioClient(m_ctx.audio.renderer);
    }
}

}
}
