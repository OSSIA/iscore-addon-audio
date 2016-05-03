#include "AudioInspector.hpp"
#include <Audio/Commands/ChangeAudioFile.hpp>
#include <Audio/Commands/ChangeSend.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>
#include <QFormLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QDialog>
#include <QLabel>
#include <core/document/Document.hpp>
#include <QDialogButtonBox>

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
namespace Send
{
InspectorWidget::InspectorWidget(
        const ProcessModel &object,
        const iscore::DocumentContext &doc,
        QWidget *parent):
    InspectorWidgetDelegate_T {object, parent},
    m_dispatcher{doc.commandStack}
{
    setObjectName("SendInspectorWidget");

    auto lay = new QFormLayout;

    /*m_edit = new QLineEdit{object.file().name()};

    con(process(), &Sound::ProcessModel::fileChanged,
        this, [&] {
        m_edit->setText(object.file().name());
    });

    connect(m_edit, &QLineEdit::editingFinished,
        this, [&] () {
        m_dispatcher.submitCommand(new Commands::ChangeAudioFile(object, m_edit->text()));
    });

    lay->addWidget(m_edit);*/
    this->setLayout(lay);
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
