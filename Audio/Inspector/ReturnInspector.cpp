#include "ReturnInspector.hpp"

#include <Audio/Commands/ChangeSend.hpp>
#include <Audio/SendProcess/SendProcessModel.hpp>

#include <core/document/Document.hpp>

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

namespace Audio
{
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
                auto str = elt->metadata().getName() + " (" + fromPath(*elt) + ")";
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
        lbl->setText(res->metadata().getName());
    }

    con(object, &Return::ProcessModel::sendChanged,
        this, [=,&object] () {
        auto res = object.send_ptr();
        lbl->setText(res ? res->metadata().getName() : "");
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
