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

        DirectMix() = default;

        DirectMix(Id<Process::ProcessModel> p) :
            process(std::move(p)),
            mix(0)
        {

        }

        DirectMix(Id<Process::ProcessModel> p, double m) :
            process(std::move(p)),
            mix(m)
        {

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

        Routing() = default;

        Routing(
            Id<Process::ProcessModel> p_in,
            Id<Process::ProcessModel> p_out) :
            in(std::move(p_in)),
            out(std::move(p_out)),
            mix(0),
            enabled(true)
        {

        }
        Routing(Id<Process::ProcessModel> p_in,
            Id<Process::ProcessModel> p_out,
            double m) :
            in(std::move(p_in)),
            out(std::move(p_out)),
            mix(m),
            enabled(true)
        {

        }

        Routing(Id<Process::ProcessModel> p_in,
            Id<Process::ProcessModel> p_out,
            double m,
            bool en) :
            in(std::move(p_in)),
            out(std::move(p_out)),
            mix(m),
            enabled(en)
        {

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
template<>
struct hash<Audio::Mix::DirectMix> {
public:
    auto operator()(const Audio::Mix::DirectMix &s) const
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, s.process);
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
template<>
struct hash<Audio::Mix::DirectMix> {
public:
    auto operator()(const Audio::Mix::DirectMix &s) const
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, s.process);
        return seed;
    }
};
}
