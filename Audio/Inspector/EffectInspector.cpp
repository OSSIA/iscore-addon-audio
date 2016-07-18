#include "EffectInspector.hpp"
#include <Audio/EffectProcess/Effect/Faust/FaustEffectModel.hpp>
#include <Audio/EffectProcess/Effect/Faust/FaustEffectFactory.hpp>
#include <Audio/Commands/InsertEffect.hpp>
#include <Audio/Commands/EditFaustEffect.hpp>

#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDialog>
#include <QListWidget>

namespace Audio
{
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
        // TODO on_titleChanged...
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
