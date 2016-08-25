#include "EffectWidget.hpp"
#include <QMenu>
#include <QPushButton>
#include <QMessageBox>

#include <Scenario/Commands/Constraint/AddProcessToConstraint.hpp>
#include <Automation/AutomationModel.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <iscore/command/Dispatchers/MacroCommandDispatcher.hpp>
#include <Audio/Commands/CreateEffectAutomation.hpp>
#include <Automation/Commands/InitAutomation.hpp>
#include <Audio/EffectProcess/Effect/Widgets/EffectSlider.hpp>

namespace Audio
{
namespace Effect
{


EffectWidget::EffectWidget(
        EffectModel& fx,
        const iscore::DocumentContext& doc,
        QWidget* parent):
    QFrame{parent},
    m_effect{fx},
    m_context{doc}
{
    setObjectName("EffectWidget");
    setStyleSheet("QFrame#EffectWidget { border: 1px solid black; border-radius: 10px; }");
    auto lay = new QVBoxLayout;
    m_layout = new QGridLayout;

    {
        auto title = new QWidget;
        auto title_lay = new QHBoxLayout;
        title->setLayout(title_lay);

        auto label = new iscore::ReactiveLabel<ModelMetadataNameParameter>(fx.metadata, this);
        label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        title_lay->addWidget(label);

        auto rm_but = new QPushButton{"x"};
        rm_but->setStyleSheet(
                    "QPushButton { "
                    "border-style: solid;"
                    "border-width: 2px;"
                    "border-color: black;"
                    "border-radius: 15px;}");
        connect(rm_but, &QPushButton::clicked,
                this, [&] () {
            iscore::clearLayout(m_layout);
            m_sliders.clear();
            emit removeRequested();
        });

        title_lay->addWidget(rm_but);

        lay->addWidget(title);
    }

    lay->addLayout(m_layout);

    this->setLayout(lay);
    lay->addStretch();

    con(fx, &EffectModel::effectChanged,
        this, &EffectWidget::setup, Qt::QueuedConnection);
    setup();
}

void EffectWidget::on_createAutomation(const State::Address& addr, double min, double max)
{

    QObject* obj = &m_effect;
    while(obj)
    {
        auto parent = obj->parent();
        if(auto cst = dynamic_cast<Scenario::ConstraintModel*>(parent))
        {
            RedoMacroCommandDispatcher<Commands::CreateEffectAutomation> macro{m_context.commandStack};
            auto make_cmd =
                    Scenario::Command::make_AddProcessToConstraint(
                        *cst,
                        Metadata<ConcreteFactoryKey_k, Automation::ProcessModel>::get());
            macro.submitCommand(make_cmd);

            auto& autom = safe_cast<Automation::ProcessModel&>(cst->processes.at(make_cmd->processId()));
            macro.submitCommand(new Automation::InitAutomation{autom, addr, min, max});

            macro.commit();

        }
        else
        {
            obj = parent;
        }
    }
}

void EffectWidget::setup()
{
    iscore::clearLayout(m_layout);
    m_sliders.clear();

    auto comp = iscore::findComponent<LocalTree::EffectComponent>(m_effect.components);
    if(!comp)
        return;

    auto& params = comp->parameters();
    for(auto& param_node : params.children())
    {
        if(param_node->getAddress())
        {
            auto slider = new EffectSlider{*param_node, this};
            m_sliders.push_back(slider);
            connect(slider, &EffectSlider::createAutomation,
                    this, &EffectWidget::on_createAutomation,
                    Qt::QueuedConnection);
        }
    }

    flow();
}

void EffectWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    reflow();
}

void EffectWidget::flow()
{
    if(m_sliders.empty())
        return;

    auto one_row = m_sliders[0]->height();
    auto max_row = this->height() - 50;
    auto row_count = std::min(1.f, std::round(std::floor(float(one_row) / float(max_row))));

    int cur_col = 0;
    int cur_row = 0;
    for(auto elt : m_sliders)
    {
        if(cur_row == row_count)
        {
            cur_row = 0;
            cur_col++;
        }

        m_layout->addWidget(elt, cur_row, cur_col);

        cur_row++;
    }
}

void EffectWidget::reflow()
{
    std::vector<QLayoutItem*> items;
    while(auto item = m_layout->takeAt(0))
    {
        items.push_back(item);
    }

    if(items.empty())
        return;

    auto one_row = std::min(30, m_sliders[0]->height());
    auto max_row = this->height();
    auto row_count = std::min(1.f, std::round(std::floor(float(one_row) / float(max_row))));

    int cur_col = 0;
    int cur_row = 0;
    for(auto elt : items)
    {
        if(cur_row == row_count)
        {
            cur_row = 0;
            cur_col++;
        }

        m_layout->addItem(elt, cur_row, cur_col);

        cur_row++;
    }


}

}
}
