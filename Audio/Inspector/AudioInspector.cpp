#include "AudioInspector.hpp"
#include <Audio/Commands/ChangeAudioFile.hpp>
#include <Audio/Commands/ChangeSend.hpp>
#include <Audio/Commands/UpdateMix.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <QFormLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <core/document/Document.hpp>
#include <QDialogButtonBox>
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
        }
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
    /*
    enum class process_type {
        Data, Send, Fx
    };
    auto get_proc_type = [=] (const Id<Process::ProcessModel&> proc) {
        auto proc = &cst->processes.at(proc);
        if(dynamic_cast<const Send::ProcessModel*>(proc))
            return process_type::Send;
        else if(dynamic_cast<const Effect::ProcessModel*>(proc))
            return process_type::Fx;
        else
            return process_type::Data;
    };
    auto get_index = [=] (const DirectMix & dmx) {
        // It is necessarily on the last row.
        auto row = n_row - 1;
        switch(get_proc_type(dmx.process))
        {
            case process_type::Data:
                for(int col = 0; col < n_data; col++)
                {
                    if(auto item = m_table->item(row, col))
                    {
                        DirectMix data = item->data(Qt::UserRole + 1).value<DirectMix>();
                        if(data.process == dmx)
                        {
                            ret
                        }

                    }
                }
                break;
            case process_type::Send:
                ISCORE_ABORT;
                break;
            case process_type::Fx:
                break;
            default:
                ISCORE_ABORT;
        }

    };
    auto get_routing_index = [] (const Routing& rtng) {

    };*/

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
            auto sb = new MixSpinBox{m_table};
            m_table->setCellWidget(row, col, sb);

            connect(sb, SignalUtils::QSpinBox_valueChanged_int,
                    this, [=,&mix] (int val) {
                m_dispatcher.submitCommand(
                            new Audio::Commands::UpdateRouting{
                                mix,
                                Routing{col_it->process, fx_it->process, val / 100.} });
            });
            /*
            auto item = new QTableWidgetItem;
            item->setText("D->FX");
            item->setBackgroundColor(Qt::darkRed);
            m_table->setItem(row, col, item);
            */
            row_labels.push_back(pretty_name(fx_it->process));
            fx_it++;
        }

        // Data -> Send
        auto send_it = mix.sends().begin();
        for(int row = n_fx; row < n_fx + n_sends; row++)
        {
            auto item = new QTableWidgetItem;
            item->setText("D->Send");
            item->setBackgroundColor(Qt::darkGreen);
            m_table->setItem(row, col, item);
            row_labels.push_back(pretty_name(*send_it));
            send_it++;
        }

        // Data -> Direct
        auto item = new QTableWidgetItem;
        item->setText("D->Direct");
        item->setBackgroundColor(Qt::darkBlue);
        m_table->setItem(n_fx + n_sends, col, item);
        row_labels.push_back(tr("Direct"));
        col_it++;
    }

    // For each fx, create relevant items.
    col_it = mix.directFx().begin();
    for(int col = n_data; col < (n_data + n_fx); col++)
    {
        col_labels.push_back(pretty_name(col_it->process));

        // Fx -> Send
        for(int row = n_fx; row < n_fx + n_sends; row++)
        {
            auto item = new QTableWidgetItem;
            item->setText("Fx->Send");
            item->setBackgroundColor(Qt::darkYellow);
            m_table->setItem(row, col, item);
        }

        // Fx -> Direct
        auto item = new QTableWidgetItem;
        item->setText("D->Direct");
        item->setBackgroundColor(Qt::darkMagenta);
        m_table->setItem(n_fx + n_sends, col, item);

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
}
