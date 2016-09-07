#include <boost/graph/graphviz.hpp>
#include <boost/range/iterator_range.hpp>
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
#include <type_traits>
#include <sstream>
#include <QMessageBox>
#include <QApplication>
namespace Audio
{
namespace AudioStreamEngine
{
AudioGraphBuilder::AudioGraphBuilder(Constraint &root)
{
    // 1. Create vertices
    root.visit(m_graph);

    // 2. For all "Return" vertices, add edge to "Send"
    // since the return depends on the send
    // TODO currently this is quadratic, it could be made linear
    // by storing the created vertice in the component
    std::vector<std::string> names;
    auto vertices = boost::vertices(m_graph);
    for(auto it = vertices.first; it != vertices.second; ++it)
    {
        QObject* asComp = m_graph[*it];
        names.push_back(asComp->objectName().toStdString());

        // For all the returns
        if(auto return_vertice = dynamic_cast<ReturnComponent*>(m_graph[*it]))
        {
            auto send = return_vertice->process().send_ptr();
            if(!send)
                continue;

            // We actually add an edge not to the send, but to the parent constraint
            // of the send.
            auto send_parent = dynamic_cast<Scenario::ConstraintModel*>(send->parent());
            if(!send_parent)
                return;


            auto& cst_comp = iscore::component<Constraint>(send_parent->components());

            // Then find the corresponding send
            for(auto it_k = vertices.first; it_k != vertices.second; ++it_k)
            {
                if(m_graph[*it_k] == &cst_comp)
                {
                    // Add an edge from return to send
                    boost::add_edge(*it_k, *it, m_graph);
                    break;
                }
            }
        }
    }

    // 3. Generate image of the graph.
    auto str = toGraphViz();
    std::cerr << str;
}

boost::optional<std::deque<int> > AudioGraphBuilder::check() const
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
        QMessageBox::warning(qApp->activeWindow(),
                             QObject::tr("Unable to play"), e.what(), QMessageBox::Ok, QMessageBox::NoButton);
        return boost::none;
    }

    return boost::none;
}

// Found on stackoverflow : http://stackoverflow.com/a/13453770/1495627
template<class It>
boost::iterator_range<It> pair_range(const std::pair<It, It>& p)
{
  return boost::make_iterator_range(p.first, p.second);
}

static void make_realtime_rec(AudioGraphVertice v, const AudioGraph& g)
{
    auto& comp = g[v];
    if(!comp->realTime())
    {
        comp->setRealTime(true);
        for(auto e : pair_range(boost::out_edges(v, g)))
        {
            make_realtime_rec(target(e, g), g);
        }
    }
}

void AudioGraphBuilder::apply(const std::deque<int>& sorted_vertices, Context& ctx)
{
    // First we check recursively for nodes that have to be real-time
    // (i.e. they imply real-time input)
    int priority = 0;
    for(auto vertice : sorted_vertices)
    {
        auto component = m_graph[vertice];

        // Set the priority
        component->priority = priority++;

        if(component->realTime())
        {
            for(auto e : pair_range(boost::out_edges(vertice, m_graph)))
            {
                make_realtime_rec(target(e, m_graph), m_graph);
            }
        }
    }

    // Then we create the streams.
    for(auto vertice : sorted_vertices)
    {
        Audio::AudioStreamEngine::Component* component = m_graph[vertice];
        // qDebug() << component->prettyName() << component->priority;

        component->makeStream(ctx);
        if(!component->getStream())
        {
            qDebug() << "Warning: component does not have a stream";
        }
    }
}

std::string AudioGraphBuilder::toGraphViz()
{
    std::stringstream s;
    boost::write_graphviz(s, m_graph, [&] (auto& out, const auto& v) {
        out << "[label=\"" << this->m_graph[v]->prettyName().toStdString() << "\"]";
    },
    [] (auto&&...) {});

    // Iterate over lambda closure members
    return s.str();
}
}
}
