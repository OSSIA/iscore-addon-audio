#include "MixInspector.hpp"
#include <Audio/Commands/UpdateMix.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <iscore/widgets/SignalUtils.hpp>

#include <QCheckBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>
namespace Audio
{
namespace Mix
{
InspectorWidget::InspectorWidget(
        const Mix::ProcessModel &object,
        const iscore::DocumentContext &doc,
        QWidget *parent):
    InspectorWidgetDelegate_T {object, parent},
    m_dispatcher{doc.commandStack}
{
    setObjectName("MixInspectorWidget");

    auto lay = new QFormLayout;
    this->setLayout(lay);

    m_table = new QTableWidget{this};
    recreate();

    con(object, &Mix::ProcessModel::structureChanged,
        this, &InspectorWidget::recreate);
    con(object, &Mix::ProcessModel::routingChanged,
        this, &InspectorWidget::updateRouting);
    con(object, &Mix::ProcessModel::directMixChanged,
        this, &InspectorWidget::updateDirectMix);
}

class MixSpinBox : public QSpinBox
{
    public:
        MixSpinBox(QWidget* parent):
            QSpinBox{parent}
        {
            setMinimum(0);
            setMaximum(100);
            setSizePolicy(QSizePolicy::Minimum, {});
            setAlignment(Qt::AlignLeft);
        }
};

class DirectMixTableWidget : public QWidget
{
    public:
        DirectMixTableWidget(
                QWidget* parent,
                QuietOngoingCommandDispatcher& dispatcher,
                const Mix::ProcessModel& mix,
                const Id<Process::ProcessModel>& column):
            QWidget{parent},
            spinBox{new MixSpinBox{this}}
        {
            auto lay = new QVBoxLayout;
            this->setLayout(lay);

            lay->addWidget(spinBox);
            setMix(mix.mix(column));

            connect(spinBox, SignalUtils::QSpinBox_valueChanged_int(),
                    this, [=,&mix,&dispatcher] (int val) {
                dispatcher.submitCommand<Audio::Commands::UpdateDirect>(
                                mix,
                                Audio::Mix::DirectMix(column, val / 100.));
            });

            connect(spinBox, &QSpinBox::editingFinished,
                    this, [=,&dispatcher] () { dispatcher.commit(); });
        }

        void setMix(double m)
        {
            spinBox->setValue(m * 100);
        }

    private:
        MixSpinBox* const spinBox{};

};

class RoutingTableWidget : public QWidget
{
    public:
        RoutingTableWidget(
                QWidget* parent,
                QuietOngoingCommandDispatcher& dispatcher,
                const Mix::ProcessModel& mix,
                const Id<Process::ProcessModel>& column,
                const Id<Process::ProcessModel>& row):
            QWidget{parent},
            checkBox{new QCheckBox{this}},
            spinBox{new MixSpinBox{this}}
        {
            auto lay = new QVBoxLayout;
            this->setLayout(lay);

            lay->addWidget(spinBox);
            lay->addWidget(checkBox);

            setMix(mix.mix(column, row));
            checkBox->setChecked(mix.routings().find(Routing{column, row})->enabled);
            connect(spinBox, SignalUtils::QSpinBox_valueChanged_int(),
                    this, [=,&mix,&dispatcher] (int val) {
                dispatcher.submitCommand<Audio::Commands::UpdateRouting>(
                                mix,
                                Routing{column, row, val / 100., bool(checkBox->checkState())});
            });

            connect(spinBox, &QSpinBox::editingFinished,
                    this, [&dispatcher] () {
                dispatcher.commit();
            });

            connect(checkBox, &QCheckBox::stateChanged,
                    this, [=,&mix,&dispatcher] (int check) {
                // TODO check if different
                dispatcher.submitCommand<Audio::Commands::UpdateRouting>(
                                mix,
                                Routing{column, row, spinBox->value() / 100., bool(check)});
                dispatcher.commit();
            });
        }

        void setMix(double m)
        {
            spinBox->setValue(m * 100);
        }


    private:
        QCheckBox* const checkBox{};
        MixSpinBox* const spinBox{};
};

void InspectorWidget::recreate()
{
    m_table->clear();
    // Row : input processes & direct outputs (fx, send, direct)
    // Column : output processes (sound, scenario, fx, etc.)
    const Mix::ProcessModel& mix = process();
    int n_data = mix.directData().size();
    int n_fx = mix.directFx().size();
    int n_sends = mix.sends().size();

    int n_row = n_fx + n_sends + 1;
    int n_col = n_data + n_fx;

    m_table->setRowCount(n_row);
    m_table->setColumnCount(n_col);

    auto cst = safe_cast<const Scenario::ConstraintModel*>(process().parent());

    // For each direct data, create relevant items.
    auto pretty_name = [&] (const Id<Process::ProcessModel>& dmx) {
        return cst->processes.at(dmx).metadata.name();
    };

    QStringList col_labels;
    QStringList row_labels;
    auto col_it = mix.directData().begin();
    for(int col = 0; col < n_data; col++)
    {
        col_labels.push_back(pretty_name(col_it->process));

        // Data -> Fx
        auto fx_it = mix.directFx().begin();
        for(int row = 0; row < n_fx; row++)
        {
            auto sb = new RoutingTableWidget{m_table, m_dispatcher, mix, col_it->process, fx_it->process};
            m_table->setCellWidget(row, col, sb);
            m_routings.insert(std::make_pair(Routing{col_it->process, fx_it->process}, sb));

            row_labels.push_back(pretty_name(fx_it->process));
            fx_it++;
        }

        // Data -> Send
        auto send_it = mix.sends().begin();
        for(int row = n_fx; row < n_fx + n_sends; row++)
        {
            auto sb = new RoutingTableWidget{m_table, m_dispatcher, mix, col_it->process, *send_it};
            m_table->setCellWidget(row, col, sb);
            m_routings.insert(std::make_pair(Routing{col_it->process, *send_it}, sb));

            row_labels.push_back(pretty_name(*send_it));
            send_it++;
        }

        // Data -> Direct
        auto sb = new DirectMixTableWidget{m_table, m_dispatcher, mix, col_it->process};
        m_table->setCellWidget(n_fx + n_sends, col, sb);
        m_directs.insert(std::make_pair(DirectMix{col_it->process}, sb));

        row_labels.push_back(tr("Direct"));
        col_it++;
    }

    // For each fx, create relevant items.
    col_it = mix.directFx().begin();
    for(int col = n_data; col < (n_data + n_fx); col++)
    {
        col_labels.push_back(pretty_name(col_it->process));

        // Fx -> Send
        auto send_it = mix.sends().begin();
        for(int row = n_fx; row < n_fx + n_sends; row++)
        {
            auto sb = new RoutingTableWidget{m_table, m_dispatcher, mix, col_it->process, *send_it};
            m_table->setCellWidget(row, col, sb);
            m_routings.insert(std::make_pair(Routing{col_it->process, *send_it}, sb));

            send_it++;
        }

        // Fx -> Direct
        auto sb = new DirectMixTableWidget{m_table, m_dispatcher, mix, col_it->process};
        m_table->setCellWidget(n_fx + n_sends, col, sb);
        m_directs.insert(std::make_pair(DirectMix{col_it->process}, sb));
        col_it++;
    }
    m_table->setHorizontalHeaderLabels(col_labels);
    m_table->setVerticalHeaderLabels(row_labels);

    this->layout()->addWidget(m_table);
}

void InspectorWidget::updateRouting(const Routing & r)
{
    auto it = m_routings.find(r);
    ISCORE_ASSERT(it != m_routings.end());

    it->second->setMix(r.mix);

}

void InspectorWidget::updateDirectMix(const DirectMix & d)
{
    auto it = m_directs.find(d);
    ISCORE_ASSERT(it != m_directs.end());

    it->second->setMix(d.mix);

}

}
}
