#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <score/command/Dispatchers/QuietOngoingCommandDispatcher.hpp>
#include <Audio/MixProcess/MixProcessModel.hpp>

class QTableWidget;


namespace Audio
{
namespace Mix
{
class DirectMixTableWidget;
class RoutingTableWidget;
class ProcessModel;

class MixWidget : public QWidget
{
        Q_OBJECT
    public:
        MixWidget(
                const Mix::ProcessModel &object,
                const score::DocumentContext &doc,
                QWidget* parent);

    signals:
        void pressed();

    private:
        void mousePressEvent(QMouseEvent* event) override;

        void recreate();
        void updateRouting(const Routing&);
        void updateDirectMix(const DirectMix&);

        const Mix::ProcessModel& m_process;
        QTableWidget* m_table{};

        const score::CommandStackFacade& m_dispatcher;

        score::hash_map<Routing, RoutingTableWidget*> m_routings;
        score::hash_map<DirectMix, DirectMixTableWidget*> m_directs;

};

class InspectorWidget final :
        public Process::InspectorWidgetDelegate_T<ProcessModel>
{
    public:
        explicit InspectorWidget(
                const ProcessModel& object,
                const score::DocumentContext& doc,
                QWidget* parent);

};
}
}
