#pragma once
#include <boost/multi_index_container.hpp>
#include <Process/Process.hpp>
#include <boost/functional/hash/hash.hpp>

namespace Audio
{
namespace Mix
{
namespace bmi = boost::multi_index;
struct DirectMix
{
        friend bool operator==(const DirectMix& mix, const Id<Process::ProcessModel>& proc)
        {
            return mix.process == proc;
        }
        friend bool operator!=(const DirectMix& mix, const Id<Process::ProcessModel>& proc)
        {
            return mix.process != proc;
        }
        friend bool operator==(const Id<Process::ProcessModel>& proc, const DirectMix& mix)
        {
            return mix.process == proc;
        }
        friend bool operator!=(const Id<Process::ProcessModel>& proc, const DirectMix& mix)
        {
            return mix.process != proc;
        }
        friend bool operator==(const DirectMix& mix, const DirectMix& proc)
        {
            return mix.process == proc.process;
        }
        friend bool operator!=(const DirectMix& mix, const DirectMix& proc)
        {
            return mix.process != proc.process;
        }

        Id<Process::ProcessModel> process;
        double mix{};
};

struct Routing
{
        friend bool operator==(const Routing& lhs, const Routing& rhs)
        {
            return lhs.in == rhs.in && lhs.out == rhs.out;
        }

        Id<Process::ProcessModel> in;
        Id<Process::ProcessModel> out;

        double mix{};
        bool enabled = true;
};

using RoutingMap = bmi::multi_index_container<
Routing,
bmi::indexed_by<
    bmi::hashed_unique<
        bmi::identity<Routing>
    >
>
>;
}
}


namespace std {
template<>
struct hash<Audio::Mix::Routing> {
public:
    auto operator()(const Audio::Mix::Routing &s) const
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, s.in);
        boost::hash_combine(seed, s.out);
        return seed;
    }
};
}


namespace boost {
template<>
struct hash<Audio::Mix::Routing> {
public:
    auto operator()(const Audio::Mix::Routing &s) const
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, s.in);
        boost::hash_combine(seed, s.out);
        return seed;
    }
};
}
