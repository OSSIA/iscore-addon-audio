#pragma once
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <Audio/EffectProcess/Effect/EffectModel.hpp>
#include <iscore/widgets/ClearLayout.hpp>
#include <boost/iterator_adaptors.hpp>
namespace Audio
{
namespace Effect
{
template<typename Property_T>
class QReactiveLabel : public QLabel
{
        QReactiveLabel(const typename Property_T::model_type& model, QWidget* parent):
            QLabel{model.get(), parent}
        {
            con(&model, Property_T::notify(),
                this, &QLabel::setText);
        }
};


struct EffectParameter
{
        int64_t id{};
        QString label;
        float min{};
        float max{};
        float init{};
};

struct AudioEffectParameterAdaptor
{
        AudioEffect& effect;
};


auto begin(AudioEffectParameterAdaptor fx)
{

}


struct EffectParameterIterator
{
        AudioEffect effect;
        int i = -1;
};

/**
 * @brief The EffectWidget class
 *
 * Controls for a single effect.
 * Show a list of sliders for each declared parameter.
 */
class EffectWidget :
        public QWidget
{
    public:
        EffectWidget(EffectModel& fx, QWidget* parent):
            QWidget{parent},
            m_effect{fx}
        {
            auto lay = new QVBoxLayout;
            m_layout = new QGridLayout;

            lay->addWidget(new QReactiveLabel<ModelMetadataNameParameter>(fx.metadata, this));
            lay->addLayout(m_layout);

            reflow();
        }

        void resizeEvent(QResizeEvent *event) override
        {

        }


    private:
        void reflow()
        {

            auto fx = m_effect.effect();
            if(!fx)
            {
                iscore::clearLayout(m_layout);
                return;
            }
            int n = fx->GetControlCount();
            for(int i = 0; i < n; i++)
            {

            }

        }

        EffectModel& m_effect;
        QGridLayout* m_layout{};
};
}
}
