#include "AudioInspector.hpp"
#include <Audio/Commands/ChangeAudioFile.hpp>

#include <QFormLayout>
#include <QLineEdit>
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
}
