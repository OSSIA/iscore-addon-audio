#pragma once
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <Audio/EffectProcess/Effect/EffectModel.hpp>

namespace Audio
{
namespace Effect
{

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

            auto lab = new QLabel;
            lab->setText(fx.metadata.getName());
            lay->addWidget(new QLabel);

            reflow();
        }

        void resizeEvent(QResizeEvent *event) override
        {

        }


    private:
        void reflow()
        {
            auto fx = m_effect.effect();
            int n = ;

        }

        EffectModel& m_effect;
        QGridLayout* m_layout{};
};
}
}
