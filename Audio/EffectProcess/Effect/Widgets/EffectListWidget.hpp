#pragma once

#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Audio/EffectProcess/Effect/Widgets/EffectWidget.hpp>
#include <iscore/widgets/ClearLayout.hpp>
#include <Audio/Commands/InsertEffect.hpp>
#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
#include <QVBoxLayout>
namespace Audio
{
namespace Effect
{
class EffectListWidget :
        public QWidget,
        public Nano::Observer
{
        const Effect::ProcessModel& m_effects;
        const iscore::DocumentContext& m_context;
        CommandDispatcher<> m_dispatcher;

        QHBoxLayout* m_layout{};

        std::vector<EffectWidget*> m_widgets;
    public:
        EffectListWidget(
                const Effect::ProcessModel& fx,
                const iscore::DocumentContext& doc,
                QWidget* parent):
            QWidget{parent},
            m_effects{fx},
            m_context{doc},
            m_dispatcher{doc.commandStack}
        {
            this->setLayout(m_layout = new QHBoxLayout);

            con(fx, &Effect::ProcessModel::effectsChanged,
                this, &EffectListWidget::setup);

            fx.effects().removing.connect<EffectListWidget, &EffectListWidget::on_effectRemoved>(this);
        }

    private:
        void on_effectRemoved(const EffectModel& fx)
        {
            auto it = find_if(m_widgets, [&] (auto ptr) { return &ptr->effect() == &fx; });
            if(it == m_widgets.end())
                return;

            auto ptr = *it;
            m_widgets.erase(it);
            m_layout->removeWidget(ptr);
            delete ptr;
        }

        void setup()
        {
            iscore::clearLayout(m_layout);
            for(auto& effect : m_effects.effects())
            {
                auto widg = new EffectWidget{effect, m_context, this};
                m_widgets.push_back(widg);
                connect(widg, &EffectWidget::removeRequested, this, [=,&effect] () {
                    auto cmd = new Commands::RemoveEffect{m_effects, effect};
                    m_dispatcher.submitCommand(cmd);
                }, Qt::QueuedConnection);
                m_layout->addWidget(widg);
            }

            m_layout->addStretch();
        }
};
}
}
