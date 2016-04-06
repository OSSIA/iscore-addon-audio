#include "AudioInspector.hpp"
#include <Audio/Commands/ChangeAudioFile.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>
#include <QFormLayout>
#include <QLineEdit>

namespace Audio
{
InspectorWidget::InspectorWidget(
        const SoundProcess::ProcessModel &object,
        const iscore::DocumentContext &doc,
        QWidget *parent):
    InspectorWidgetDelegate_T {object, parent},
    m_dispatcher{doc.commandStack}
{
    setObjectName("AudioInspectorWidget");

    auto lay = new QFormLayout;

    m_edit = new QLineEdit{object.file()};

    con(process(), &SoundProcess::ProcessModel::fileChanged,
        this, [&] (const QString& str) {
        m_edit->setText(str);
    });

    connect(m_edit, &QLineEdit::editingFinished,
        this, [&] () {
        m_dispatcher.submitCommand(new Commands::ChangeAudioFile(object, m_edit->text()));
    });

    lay->addWidget(m_edit);
    this->setLayout(lay);
}
}
