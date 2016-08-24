#include "EffectWidget.hpp"
#include <QContextMenuEvent>
#include <QMenu>
#include <QPushButton>
#include <QMessageBox>

namespace Audio
{
namespace Effect
{

class EffectSlider : public QWidget
{
    public:
        EffectSlider(const ossia::net::node_base& fx, QWidget* parent):
            QWidget{parent},
            m_param{fx}
        {
            auto addr = m_param.getAddress();
            auto dom = addr->getDomain();

            if(auto f = ossia::net::min(dom).try_get<ossia::Float>()) m_min = f->value;
            if(auto f = ossia::net::max(dom).try_get<ossia::Float>()) m_max = f->value;

            auto lay = new QVBoxLayout;
            lay->addWidget(new QLabel{QString::fromStdString(m_param.getName())});
            m_slider = new iscore::DoubleSlider{this};
            lay->addWidget(m_slider);
            this->setLayout(lay);


            connect(m_slider, &iscore::DoubleSlider::valueChanged,
                    this, [=] (double v)
            {
                // v is between 0 - 1
                m_param.getAddress()->pushValue(ossia::Float{m_min + (m_max - m_min) * v});
            });

            m_callback = addr->add_callback([=] (const ossia::value& val)
            {
                if(auto v = val.try_get<ossia::Float>())
                {
                    m_slider->setValue(v->value);
                }
            });

            m_addAutomAction = new QAction{tr("Add automation")};
            connect(m_addAutomAction, &QAction::triggered,
                    this, &EffectSlider::on_addAutomation);
            // TODO show tooltip with current value
        }

        ~EffectSlider()
        {
            if(auto addr = m_param.getAddress())
            {
                addr->remove_callback(m_callback);
            }
        }

    private:
        void on_addAutomation()
        {
          auto addr = m_param.getAddress()->getTextualAddress();
          QMessageBox::warning(nullptr, "", QString::fromStdString(addr));
        }

        void contextMenuEvent(QContextMenuEvent* event) override
        {
          QMenu* contextMenu = new QMenu{this};

          contextMenu->addAction(m_addAutomAction);
          contextMenu->exec(event->globalPos());

          contextMenu->deleteLater();
        }

        const ossia::net::node_base& m_param;
        ossia::net::address_base::callback_index m_callback;
        float m_min{0.};
        float m_max{1.};

        iscore::DoubleSlider* m_slider{};
        QAction* m_addAutomAction{};
};



EffectWidget::EffectWidget(EffectModel& fx, QWidget* parent):
    QFrame{parent},
    m_effect{fx}
{
    setObjectName("EffectWidget");
    setStyleSheet("QFrame#EffectWidget { border: 1px solid black; border-radius: 10px; }");
    auto lay = new QVBoxLayout;
    m_layout = new QGridLayout;

    {
        auto title = new QWidget;
        auto title_lay = new QHBoxLayout;
        title->setLayout(title_lay);

        auto label = new QReactiveLabel<ModelMetadataNameParameter>(fx.metadata, this);
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
            m_sliders.push_back(new EffectSlider{*param_node, this});
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
