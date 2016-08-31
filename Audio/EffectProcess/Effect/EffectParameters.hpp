#pragma once
#include <QString>
#include <iterator>
#include <LibAudioStreamMC++.h>

namespace Audio
{
namespace Effect
{
struct EffectParameter
{
        EffectParameter() = default;
        EffectParameter(const EffectParameter&) = default;
        EffectParameter(EffectParameter&&) = default;
        EffectParameter& operator=(const EffectParameter&) = default;
        EffectParameter& operator=(EffectParameter&&) = default;
        int64_t id{};
        float min{};
        float max{};
        float init{};

        QString label{};
};

struct AudioEffectParameterAdaptor
{
        AudioEffect effect;
};

struct EffectParameterIterator final :
    public std::iterator<
        std::input_iterator_tag,
        EffectParameter,
        int64_t,
        const EffectParameter*,
        EffectParameter>
{
        AudioEffectParameterAdaptor effect;
        int64_t num = 0;
        EffectParameter param;

    public:
        EffectParameterIterator(const EffectParameterIterator& other) = default;
        EffectParameterIterator(EffectParameterIterator&& other) = default;
        EffectParameterIterator& operator=(const EffectParameterIterator& other) = default;
        EffectParameterIterator& operator=(EffectParameterIterator&& other) = default;

        explicit EffectParameterIterator(
                AudioEffectParameterAdaptor p,
                int64_t n = 0) :
            effect{p}, num{n}, param{readEffect()}
        {

        }

        EffectParameterIterator& operator++()
        {
            ++num;
            param = readEffect();
            return *this;
        }

        EffectParameterIterator operator++(int)
        {
            EffectParameterIterator retval{*this};
            ++(*this);
            return retval;
        }

        bool operator==(const EffectParameterIterator& other) const
        {
            return num == other.num;
        }

        bool operator!=(const EffectParameterIterator& other) const
        {
            return !(*this == other);
        }

        const EffectParameter& operator*() const
        {
            return param;
        }

    private:
        EffectParameter readEffect()
        {
            EffectParameter e;
            if(num < GetControlCountEffect(effect.effect))
            {
                char buf[512]{};
                GetControlParamEffect(effect.effect, num, buf, &e.min, &e.max, &e.init);
                e.label = buf;
                e.id = num;
            }
            return e;
        }
};

inline EffectParameterIterator begin(const AudioEffectParameterAdaptor& fx)
{
    return EffectParameterIterator(fx, 0);
}
inline EffectParameterIterator end(const AudioEffectParameterAdaptor& fx)
{
    return EffectParameterIterator(fx, GetControlCountEffect(fx.effect));
}
}
}
