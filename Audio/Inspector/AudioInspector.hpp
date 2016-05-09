#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
#include <iscore/command/Dispatchers/OngoingCommandDispatcher.hpp>
#include <Audio/SoundProcess/SoundProcessMetadata.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>
#include <Audio/MixProcess/MixProcessModel.hpp>
#include <Audio/ReturnProcess/ReturnProcessModel.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>
class QTableWidget;
class QListWidget;
class QLineEdit;
class QPushButton;

namespace iscore {
class Document;
}  // namespace iscore

namespace Audio
{
namespace Sound
{
class ProcessModel;
class InspectorWidget final :
        public Process::InspectorWidgetDelegate_T<Sound::ProcessModel>
{
    public:
        explicit InspectorWidget(
                const ProcessModel& object,
                const iscore::DocumentContext& doc,
                QWidget* parent);

    private:
        QLineEdit* m_edit{};
        CommandDispatcher<> m_dispatcher;
};
}


// MOVEME
namespace Mix
{
class ProcessModel;
class InspectorWidget final :
        public Process::InspectorWidgetDelegate_T<ProcessModel>
{
    public:
        explicit InspectorWidget(
                const ProcessModel& object,
                const iscore::DocumentContext& doc,
                QWidget* parent);


    private:
        void recreate();

        QTableWidget* m_table{};
        OngoingCommandDispatcher m_dispatcher;
};
}


// MOVEME
namespace Return
{
class ProcessModel;
class InspectorWidget final :
        public Process::InspectorWidgetDelegate_T<ProcessModel>
{
    public:
        explicit InspectorWidget(
                const ProcessModel& object,
                const iscore::DocumentContext& doc,
                QWidget* parent);

    private:
        CommandDispatcher<> m_dispatcher;
};
}


namespace Effect
{
class ProcessModel;
class InspectorWidget final :
        public Process::InspectorWidgetDelegate_T<ProcessModel>
{
    public:
        explicit InspectorWidget(
                const ProcessModel& object,
                const iscore::DocumentContext& doc,
                QWidget* parent);

    private:
        void recreate();
        QListWidget* m_list{};
        QPushButton* m_add{};
        CommandDispatcher<> m_dispatcher;
};
}
}
