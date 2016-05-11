#include "AudioInspector.hpp"
#include <Audio/Commands/ChangeAudioFile.hpp>
#include <Audio/Commands/ChangeSend.hpp>
#include <Audio/Commands/UpdateMix.hpp>
#include <Audio/Commands/InsertEffect.hpp>
#include <Audio/Commands/EditFaustEffect.hpp>
#include <Audio/EffectProcess/FaustEffectModel.hpp>
#include <Audio/EffectProcess/FaustEffectFactory.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <QFormLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <QPlainTextEdit>
#include <core/document/Document.hpp>
#include <QDialogButtonBox>
#include <QPushButton>
#include <Audio/SendProcess/SendProcessModel.hpp>
#include <QHeaderView>
#include <iscore/widgets/SpinBoxes.hpp>
#include <iscore/widgets/SignalUtils.hpp>
namespace Audio
{
namespace Sound
{
InspectorWidget::InspectorWidget(
        const Sound::ProcessModel &object,
        const iscore::DocumentContext &doc,
        QWidget *parent):
    InspectorWidgetDelegate_T {object, parent},
    m_dispatcher{doc.commandStack}
{
    setObjectName("SoundInspectorWidget");

    auto lay = new QFormLayout;

    m_edit = new QLineEdit{object.file().name()};

    con(process(), &Sound::ProcessModel::fileChanged,
        this, [&] {
        m_edit->setText(object.file().name());
    });

    connect(m_edit, &QLineEdit::editingFinished,
        this, [&] () {
        m_dispatcher.submitCommand(new Commands::ChangeAudioFile(object, m_edit->text()));
    });

    lay->addWidget(m_edit);
    this->setLayout(lay);
}
}

// MOVEME
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

class RoutingTableWidget : public QWidget
{
    public:
        RoutingTableWidget(
                QWidget* parent,
                OngoingCommandDispatcher& dispatcher,
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

            spinBox->setValue(mix.mix(column, row) * 100);
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
                dispatcher.submitCommand<Audio::Commands::UpdateRouting>(
                                mix,
                                Routing{column, row, spinBox->value() / 100., bool(check)});

            } );
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

        return cst->processes.at(dmx).objectName()
                + " ("
                + QString::number(*dmx.val())
                + ")";
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

            row_labels.push_back(pretty_name(fx_it->process));
            fx_it++;
        }

        // Data -> Send
        auto send_it = mix.sends().begin();
        for(int row = n_fx; row < n_fx + n_sends; row++)
        {
            auto sb = new RoutingTableWidget{m_table, m_dispatcher, mix, col_it->process, *send_it};
            m_table->setCellWidget(row, col, sb);

            row_labels.push_back(pretty_name(*send_it));
            send_it++;
        }

        // Data -> Direct
        auto sb = new MixSpinBox{m_table};
        sb->setValue(mix.mix(col_it->process));
        m_table->setCellWidget(n_fx + n_sends, col, sb);

        connect(sb, SignalUtils::QSpinBox_valueChanged_int(),
                this, [=,&mix] (int val) {
            m_dispatcher.submitCommand<Audio::Commands::UpdateDirect>(
                            mix,
                            DirectMix{col_it->process, val / 100.});
        });
        connect(sb, &QSpinBox::editingFinished,
                this, [=] () { m_dispatcher.commit(); });


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

            send_it++;
        }

        // Fx -> Direct
        auto sb = new MixSpinBox{m_table};
        sb->setValue(mix.mix(col_it->process));
        m_table->setCellWidget(n_fx + n_sends, col, sb);

        connect(sb, SignalUtils::QSpinBox_valueChanged_int(),
                this, [=,&mix] (int val) {
            m_dispatcher.submitCommand<Audio::Commands::UpdateDirect>(
                            mix,
                            DirectMix{col_it->process, val / 100.});
        });
        connect(sb, &QSpinBox::editingFinished,
                this, [=] () { m_dispatcher.commit(); });

        col_it++;
    }
    m_table->setHorizontalHeaderLabels(col_labels);
    m_table->setVerticalHeaderLabels(row_labels);

    this->layout()->addWidget(m_table);
}
}

// MOVEME
namespace Return
{
struct SendEnumeratorDialog : public QDialog
{
        QList<Send::ProcessModel*> m_sends;
        QStringList m_sendNames;
        Send::ProcessModel* m_selected{};

    public:
        SendEnumeratorDialog(const iscore::DocumentContext & doc):
            m_sends(doc.document.findChildren<Send::ProcessModel*>())
        {
            for(auto elt : m_sends)
            {
                auto str = elt->metadata.name() + " (" + fromPath(*elt) + ")";
                m_sendNames.push_back(str);
            }

            auto lay = new QVBoxLayout;
            this->setLayout(lay);
            auto lw = new QListWidget;

            lw->addItems(m_sendNames);

            lay->addWidget(lw);

            auto bbox = new QDialogButtonBox{
                    QDialogButtonBox::Ok | QDialogButtonBox::Cancel};
            lay->addWidget(bbox);
            connect(bbox, &QDialogButtonBox::accepted,
                    this, [=] () {
                m_selected = m_sends.at(lw->currentRow());
                this->accept();
            });
            connect(bbox, &QDialogButtonBox::rejected,
                    this, &QDialog::reject);
        }

        Path<Send::ProcessModel> selectedSend() const
        {
            if(m_selected)
                return *m_selected;
            return {};
        }

    private:
        QString fromPath(const Path<Send::ProcessModel>& path)
        {
            return path.unsafePath().toString();
        }
};

InspectorWidget::InspectorWidget(
        const Return::ProcessModel &object,
        const iscore::DocumentContext &doc,
        QWidget *parent):
    InspectorWidgetDelegate_T {object, parent},
    m_dispatcher{doc.commandStack}
{
    setObjectName("ReturnInspectorWidget");

    auto lay = new QFormLayout;

    auto lbl = new QLabel;
    lay->addRow(tr("Send: "), lbl);
    auto but = new QPushButton{tr("Select send")};
    lay->addRow(but);

    if(auto res = object.send_ptr())
    {
        lbl->setText(res->metadata.name());
    }

    con(object, &Return::ProcessModel::sendChanged,
        this, [=,&object] () {
        auto res = object.send_ptr();
        lbl->setText(res ? res->metadata.name() : "");
    });

    connect(but, &QPushButton::pressed,
            this, [&] () {
        SendEnumeratorDialog dial{doc};
        auto res = dial.exec();
        if(res)
        {
            m_dispatcher.submitCommand(new Commands::ChangeSend{object, dial.selectedSend()});
        }
    });

    this->setLayout(lay);
}
}

namespace Effect
{
struct FaustEditDialog : public QDialog
{
        const FaustEffectModel& m_effect;

        QPlainTextEdit* m_textedit{};
    public:
        FaustEditDialog(const FaustEffectModel& fx):
            m_effect{fx}
        {
            auto lay = new QVBoxLayout;
            this->setLayout(lay);

            m_textedit = new QPlainTextEdit{m_effect.text()};

            lay->addWidget(m_textedit);
            auto bbox = new QDialogButtonBox{
                    QDialogButtonBox::Ok | QDialogButtonBox::Cancel};
            lay->addWidget(bbox);
            connect(bbox, &QDialogButtonBox::accepted,
                    this, &QDialog::accept);
            connect(bbox, &QDialogButtonBox::rejected,
                    this, &QDialog::reject);
        }

        QString text() const
        {
            return m_textedit->document()->toPlainText();
        }
};

InspectorWidget::InspectorWidget(
        const Effect::ProcessModel &object,
        const iscore::DocumentContext &doc,
        QWidget *parent):
    InspectorWidgetDelegate_T {object, parent},
    m_dispatcher{doc.commandStack}
{
    setObjectName("EffectInspectorWidget");

    auto lay = new QHBoxLayout;
    m_list = new QListWidget;
    lay->addWidget(m_list);
    con(process(), &Effect::ProcessModel::effectsChanged,
        this, &InspectorWidget::recreate);

    recreate();

    // Add an effect
    m_add = new QPushButton{tr("Add")};
    connect(m_add, &QPushButton::pressed,
            this, [=] () {

        m_dispatcher.submitCommand(
                    new Commands::InsertEffect{
                        process(),
                        FaustEffectFactory::static_concreteFactoryKey(),
                        "",
                        0});
    });

    lay->addWidget(m_add);
    // Remove an effect
    // Effects changed
    // Effect list
    // Double-click : open editor window.

    this->setLayout(lay);
}

void InspectorWidget::recreate()
{
    m_list->clear();

    for(const auto& fx_id : process().effectsOrder())
    {
        EffectModel& fx = process().effects().at(fx_id);
        auto item = new QListWidgetItem(fx.title(), m_list);
        item->setData(Qt::UserRole, QVariant::fromValue(fx_id));
        m_list->addItem(item);
    }

    /* TODO :
    connect(m_list, &QListWidget::doubleClicked,
            this, [] () {
        // Find factory of effect.
        // Ask it to create a GUI.
    });
    */


    connect(m_list, &QListWidget::itemDoubleClicked,
            this, [=] (QListWidgetItem* item) {
        // Make a text dialog to edit faust program.
        auto id = item->data(Qt::UserRole).value<Id<EffectModel>>();
        auto faust = safe_cast<FaustEffectModel*>(&process().effects().at(id));

        FaustEditDialog edit{*faust};
        auto res = edit.exec();
        if(res)
        {
            m_dispatcher.submitCommand(new Commands::EditFaustEffect{*faust, edit.text()});
        }
    });

}
}

}
