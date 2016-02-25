#include "AudioInspector.hpp"
#include <Audio/AudioProcessModel.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>
#include <QFormLayout>
#include <QLineEdit>

namespace Audio {
const CommandParentFactoryKey& CommandFactoryName();


const CommandParentFactoryKey& CommandFactoryName() {
    static const CommandParentFactoryKey key{"Audio"};
    return key;
}

class ChangeFile : public iscore::SerializableCommand
{
           ISCORE_COMMAND_DECL(Audio::CommandFactoryName(), ChangeFile, "Change audio file")
    public:
        ChangeFile(
                Path<ProcessModel>&& model,
                const QString& text):
            m_model{std::move(model)},
            m_new{text}
        {
            m_old = m_model.find().file();
        }

        void undo() const override
        {
            m_model.find().setFile(m_old);
        }

        void redo() const override
        {
            m_model.find().setFile(m_new);
        }


    protected:
        void serializeImpl(DataStreamInput & s) const override
        {
            s << m_model << m_old << m_new;
        }
        void deserializeImpl(DataStreamOutput & s) override
        {
            s >> m_model >> m_old >> m_new;
        }

    private:
        Path<ProcessModel> m_model;
        QString m_old, m_new;
};

InspectorWidget::InspectorWidget(
        const ProcessModel &object,
        const iscore::DocumentContext &doc,
        QWidget *parent):
    InspectorWidgetDelegate_T {object, parent},
    m_dispatcher{doc.commandStack}
{
    setObjectName("AudioInspectorWidget");

    auto lay = new QFormLayout;

    m_edit = new QLineEdit{object.file()};

    con(process(), &ProcessModel::fileChanged,
        this, [&] (const QString& str) {
        m_edit->setText(str);
    });

    connect(m_edit, &QLineEdit::editingFinished,
        this, [&] () {
        m_dispatcher.submitCommand(new ChangeFile(object, m_edit->text()));
    });

    lay->addWidget(m_edit);
    this->setLayout(lay);
}


InspectorFactory::InspectorFactory()
{

}

InspectorFactory::~InspectorFactory()
{

}

Process::InspectorWidgetDelegate* InspectorFactory::make(
        const Process::ProcessModel& process,
        const iscore::DocumentContext& doc,
        QWidget* parent) const
{
    return new InspectorWidget{
        static_cast<const Audio::ProcessModel&>(process),
                doc,
                parent};
}

bool InspectorFactory::matches(const Process::ProcessModel& process) const
{
    return dynamic_cast<const Audio::ProcessModel*>(&process);
}


}
