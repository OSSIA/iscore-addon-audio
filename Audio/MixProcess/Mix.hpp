#pragma once
#include <boost/multi_index_container.hpp>
#include <Process/Process.hpp>

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
        auto h1 = id_hash<Process::ProcessModel>()(s.in);
        auto h2 = id_hash<Process::ProcessModel>()(s.out);
        return h1 ^ ( h2 << 1 );
    }
};
}


namespace boost {
template<>
struct hash<Audio::Mix::Routing> {
public:
    auto operator()(const Audio::Mix::Routing &s) const
    {
        auto h1 = id_hash<Process::ProcessModel>()(s.in);
        auto h2 = id_hash<Process::ProcessModel>()(s.out);
        return h1 ^ ( h2 << 1 );
    }
};
}