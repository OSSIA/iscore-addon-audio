#pragma once
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <iscore/widgets/DoubleSlider.hpp>
#include <Audio/EffectProcess/Effect/EffectModel.hpp>
#include <Audio/EffectProcess/LocalTree/LocalTreeEffectComponent.hpp>
#include <iscore/widgets/ClearLayout.hpp>
#include <iscore/widgets/ReactiveLabel.hpp>
#include <boost/iterator_adaptors.hpp>

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
            effect{p}, num{n}
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

        bool operator==(EffectParameterIterator other) const
        {
            return num == other.num;
        }

        bool operator!=(EffectParameterIterator other) const
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
            if(num < effect.effect->GetControlCount())
            {
                char buf[512]{};
                effect.effect->GetControlParam(num, buf, &e.min, &e.max, &e.init);
                e.label = buf;
            }
            return e;
        }
};

inline EffectParameterIterator begin(AudioEffectParameterAdaptor fx)
{
    return EffectParameterIterator(fx, 0);
}
inline EffectParameterIterator end(AudioEffectParameterAdaptor fx)
{
    return EffectParameterIterator(fx, fx.effect->GetControlCount());
}
class EffectSlider;
/**
 * @brief The EffectWidget class
 *
 * Controls for a single effect.
 * Show a list of sliders for each declared parameter.
 */
class EffectWidget :
        public QFrame
{
        Q_OBJECT
    public:
        EffectWidget(EffectModel& fx, QWidget* parent);

        auto& effect() const { return m_effect; }
    signals:
        void removeRequested();

    private:
        void setup();

        void resizeEvent(QResizeEvent *event) override;

        void flow();

        void reflow();

        EffectModel& m_effect;
        QGridLayout* m_layout{};
        std::vector<EffectSlider*> m_sliders;
};
}
}
