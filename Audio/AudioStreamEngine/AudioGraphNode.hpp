#pragma once
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/topological_sort.hpp>

namespace Audio
{
namespace AudioStreamEngine
{
class Component;
using AudioGraphNode = Audio::AudioStreamEngine::Component*;

using AudioGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, AudioGraphNode> ;
using AudioGraphVertice = decltype(boost::add_vertex(AudioGraphNode{}, std::declval<AudioGraph&>()));

}
}
