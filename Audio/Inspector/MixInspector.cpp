#include "MixInspector.hpp"
#include <Audio/Commands/UpdateMix.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <iscore/widgets/SignalUtils.hpp>
#include <iscore/widgets/MarginLess.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <QCheckBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QFormLayout>
namespace Audio
{
namespace Mix
{
InspectorWidget::InspectorWidget(
        const Mix::ProcessModel &object,
        const iscore::DocumentContext &doc,
        QWidget *parent):
    InspectorWidgetDelegate_T {object, parent}
{
    setObjectName("MixInspectorWidget");
    this->setLayout(new QVBoxLayout);
    this->layout()->addWidget(new MixWidget{object, doc, this});

}

class MixSpinBox : public QSpinBox
{
    public:
        MixSpinBox(QWidget* parent):
            QSpinBox{parent}
        {
            setMinimum(0);
            setMaximum(100);
            setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
            setAlignment(Qt::AlignLeft);
        }
};

class DirectMixTableWidget : public QWidget
{
    public:
        DirectMixTableWidget(
                QWidget* parent,
                const iscore::CommandStackFacade& stack,
                const Mix::ProcessModel& mix,
                const Id<Process::ProcessModel>& column):
            QWidget{parent},
            spinBox{new MixSpinBox{this}},
            dispatcher{stack}
        {
            auto lay = new QHBoxLayout;
            this->setContentsMargins(0, 0, 0, 0);
            this->setLayout(lay);

            lay->addWidget(spinBox);
            setMix(mix.mix(column));

            connect(spinBox, SignalUtils::QSpinBox_valueChanged_int(),
                    this, [=,&mix] (int val) {
                dispatcher.submitCommand<Audio::Commands::UpdateDirect>(
                                mix,
                                Audio::Mix::DirectMix(column, val / 100.));
            });

            connect(spinBox, &QSpinBox::editingFinished,
                    this, [=] () { dispatcher.commit(); });
        }

        void setMix(double m)
        {
            spinBox->setValue(m * 100);
        }

    private:
        MixSpinBox* const spinBox{};
        QuietOngoingCommandDispatcher dispatcher;

};

class RoutingTableWidget : public QWidget
{
    public:
        RoutingTableWidget(
                QWidget* parent,
                const iscore::CommandStackFacade& stack,
                const Mix::ProcessModel& mix,
                const Id<Process::ProcessModel>& column,
                const Id<Process::ProcessModel>& row):
            QWidget{parent},
            checkBox{new QCheckBox{this}},
            spinBox{new MixSpinBox{this}},
            dispatcher{stack}
        {
            auto lay = new QHBoxLayout;
            this->setContentsMargins(0, 0, 0, 0);
            this->setLayout(lay);

            lay->addWidget(spinBox);
            lay->addWidget(checkBox);

            const Routing& routing = *mix.routings().find(Routing{column, row});
            setMix(routing.mix);
            setEnabled(routing.enabled);

            connect(spinBox, SignalUtils::QSpinBox_valueChanged_int(),
                    this, [=,&mix] (int val) {
                dispatcher.submitCommand<Audio::Commands::UpdateRouting>(
                                mix,
                                Routing{column, row, val / 100., bool(checkBox->checkState())});
            });

            connect(spinBox, &QSpinBox::editingFinished,
                    this, [=] () {
                dispatcher.commit();
            });

            connect(checkBox, &QCheckBox::stateChanged,
                    this, [=,&mix] (int check) {
                const Routing& routing = *mix.routings().find(Routing{column, row});
                if(routing.enabled != check)
                {
                    dispatcher.submitCommand<Audio::Commands::UpdateRouting>(
                                mix,
                                Routing{column, row, spinBox->value() / 100., bool(check)});
                    dispatcher.commit();
                }
            });
        }

        void setMix(double m)
        {
            spinBox->setValue(m * 100);
        }

        void setEnabled(bool b)
        {
            checkBox->setChecked(b);
        }


    private:
        QCheckBox* const checkBox{};
        MixSpinBox* const spinBox{};
        QuietOngoingCommandDispatcher dispatcher;
};

MixWidget::MixWidget(
        const Mix::ProcessModel &object,
        const iscore::DocumentContext &doc,
        QWidget* parent):
    QWidget{parent},
    m_process{object},
    m_dispatcher{doc.commandStack}
{
    auto lay = new iscore::MarginLess<QVBoxLayout>();
    this->setLayout(lay);


    m_table = new QTableWidget{this};
    m_table->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    m_table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    lay->addWidget(m_table, 4);
    recreate();

    con(object, &Mix::ProcessModel::structureChanged,
        this, &MixWidget::recreate);
    con(object, &Mix::ProcessModel::routingChanged,
        this, &MixWidget::updateRouting);
    con(object, &Mix::ProcessModel::directMixChanged,
        this, &MixWidget::updateDirectMix);
}


void MixWidget::recreate()
{
    m_table->clear();
    m_routings.clear();
    m_directs.clear();
    // Row : input processes & direct outputs (fx, send, direct)
    // Column : output processes (sound, scenario, fx, etc.)
    int n_data = m_process.directData().size();
    int n_fx = m_process.directFx().size();
    int n_sends = m_process.sends().size();

    int n_row = n_fx + n_sends + 1;
    int n_col = n_data + n_fx;

    m_table->setRowCount(n_row);
    m_table->setColumnCount(n_col);

    auto cst = safe_cast<const Scenario::ConstraintModel*>(m_process.parent());

    // For each direct data, create relevant items.
    auto pretty_name = [&] (const Id<Process::ProcessModel>& dmx) {
        return cst->processes.at(dmx).metadata.name();
    };

    QStringList col_labels;
    QStringList row_labels;
    auto col_it = m_process.directData().begin();
    for(int col = 0; col < n_data; col++)
    {
        col_labels.push_back(pretty_name(col_it->process));

        // Data -> Fx
        {
            auto fx_it = m_process.directFx().begin();
            for(int row = 0; row < n_fx; row++)
            {
                auto sb = new RoutingTableWidget{m_table, m_dispatcher, m_process, col_it->process, fx_it->process};
                m_table->setCellWidget(row, col, sb);
                m_routings.insert(std::make_pair(Routing{col_it->process, fx_it->process}, sb));

                row_labels.push_back(pretty_name(fx_it->process));
                fx_it++;
            }
        }

        // Data -> Send
        {
            auto send_it = m_process.sends().begin();
            for(int row = n_fx; row < n_fx + n_sends; row++)
            {
                auto sb = new RoutingTableWidget{m_table, m_dispatcher, m_process, col_it->process, *send_it};
                m_table->setCellWidget(row, col, sb);
                m_routings.insert(std::make_pair(Routing{col_it->process, *send_it}, sb));

                row_labels.push_back(pretty_name(*send_it));
                send_it++;
            }
        }

        // Data -> Direct
        auto sb = new DirectMixTableWidget{m_table, m_dispatcher, m_process, col_it->process};
        m_table->setCellWidget(n_fx + n_sends, col, sb);
        m_directs.insert(std::make_pair(DirectMix{col_it->process}, sb));

        row_labels.push_back(tr("Direct"));
        col_it++;
    }

    // For each fx, create relevant items.
    col_it = m_process.directFx().begin();
    for(int col = n_data; col < (n_data + n_fx); col++)
    {
        col_labels.push_back(pretty_name(col_it->process));

        // Fx -> Fx
        {
            // Each Fx should be able to mix into all others, but not into itself.
            auto row_it = m_process.directFx().begin();
            for(int row = 0; row < n_fx; row++)
            {
                if(col_it->process != row_it->process)
                {
                    auto sb = new RoutingTableWidget{m_table, m_dispatcher, m_process, col_it->process, row_it->process};
                    m_table->setCellWidget(row, col, sb);
                    m_routings.insert(std::make_pair(Routing{col_it->process, row_it->process}, sb));
                }

                row_it++;
            }
        }

        // Fx -> Send
        {
            auto send_it = m_process.sends().begin();
            for(int row = n_fx; row < n_fx + n_sends; row++)
            {
                auto sb = new RoutingTableWidget{m_table, m_dispatcher, m_process, col_it->process, *send_it};
                m_table->setCellWidget(row, col, sb);
                m_routings.insert(std::make_pair(Routing{col_it->process, *send_it}, sb));

                send_it++;
            }
        }

        // Fx -> Direct
        auto sb = new DirectMixTableWidget{m_table, m_dispatcher, m_process, col_it->process};
        m_table->setCellWidget(n_fx + n_sends, col, sb);
        m_directs.insert(std::make_pair(DirectMix{col_it->process}, sb));
        col_it++;
    }
    m_table->setHorizontalHeaderLabels(col_labels);
    m_table->setVerticalHeaderLabels(row_labels);

    m_table->resizeRowsToContents();
    m_table->resizeColumnsToContents();
    m_table->setMinimumHeight(m_table->rowCount() * m_table->rowHeight(1));
}

void MixWidget::updateRouting(const Routing & r)
{
    auto it = m_routings.find(r);
    ISCORE_ASSERT(it != m_routings.end());

    it->second->setMix(r.mix);
    it->second->setEnabled(r.enabled);

}

void MixWidget::updateDirectMix(const DirectMix & d)
{
    auto it = m_directs.find(d);
    ISCORE_ASSERT(it != m_directs.end());

    it->second->setMix(d.mix);
}

}
}
