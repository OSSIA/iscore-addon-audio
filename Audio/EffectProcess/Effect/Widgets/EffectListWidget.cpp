#include "EffectListWidget.hpp"

#include <QVBoxLayout>
#include <Audio/EffectProcess/Effect/Widgets/EffectWidget.hpp>
#include <iscore/widgets/ClearLayout.hpp>
#include <Audio/Commands/InsertEffect.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>


namespace Audio
{
namespace Effect
{
EffectListWidget::EffectListWidget(
        const ProcessModel &fx,
        const iscore::DocumentContext &doc,
        QWidget *parent):
    QWidget{parent},
    m_effects{fx},
    m_context{doc},
    m_dispatcher{doc.commandStack}
{
    this->setLayout(m_layout = new QHBoxLayout);

    con(fx, &Effect::ProcessModel::effectsChanged,
        this, &EffectListWidget::setup);

    fx.effects().removing.connect<EffectListWidget, &EffectListWidget::on_effectRemoved>(this);

    setup();
}

void EffectListWidget::on_effectRemoved(const EffectModel &fx)
{
    auto it = find_if(m_widgets, [&] (auto ptr) { return &ptr->effect() == &fx; });
    if(it == m_widgets.end())
        return;

    auto ptr = *it;
    m_widgets.erase(it);
    m_layout->removeWidget(ptr);
    delete ptr;
}

void EffectListWidget::setup()
{
    iscore::clearLayout(m_layout);
    for(auto& effect : m_effects.effects())
    {
        auto widg = new EffectWidget{effect, m_context, this};
        m_widgets.push_back(widg);
        connect(widg, &EffectWidget::removeRequested, this, [=,&effect] ()
        {
            auto cmd = new Commands::RemoveEffect{m_effects, effect};
            m_dispatcher.submitCommand(cmd);
        }, Qt::QueuedConnection);

        connect(widg, &EffectWidget::sig_performInsert,
                this,  [=,&effect] (
                const Path<Effect::EffectModel>& fx1_p,
                const Path<Effect::EffectModel>& fx2_p,
                bool after)
        {
            auto& fx1 = fx1_p.find();
            auto& fx2 = fx2_p.find();
            if(fx1.parent() == fx2.parent() && fx1.parent() == &m_effects)
            {
                int new_pos = m_effects.effectPosition(fx2.id());
                if(after)
                    new_pos++;

                auto cmd = new Commands::MoveEffect{m_effects, fx1.id(), new_pos};
                m_dispatcher.submitCommand(cmd);
            }
        }, Qt::QueuedConnection);
        m_layout->addWidget(widg);
    }

    m_layout->addStretch();
}

}

}
