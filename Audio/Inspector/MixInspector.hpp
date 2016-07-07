#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <iscore/command/Dispatchers/OngoingCommandDispatcher.hpp>
#include <Audio/MixProcess/MixProcessModel.hpp>

class QTableWidget;

// MOVEME
class QuietOngoingCommandDispatcher final : public ICommandDispatcher
{
    public:
        QuietOngoingCommandDispatcher(const iscore::CommandStackFacade& stack):
            ICommandDispatcher{stack}
        {

        }

        template<typename TheCommand, typename... Args>
        void submitCommand(Args&&... args)
        {
            if(!m_cmd)
            {
                stack().disableActions();
                m_cmd = std::make_unique<TheCommand>(std::forward<Args>(args)...);
            }
            else
            {
                ISCORE_ASSERT(m_cmd->key() == TheCommand::static_key());
                safe_cast<TheCommand*>(m_cmd.get())->update(std::forward<Args>(args)...);
            }
        }

        void commit()
        {
            if(m_cmd)
            {
                SendStrategy::Simple::send(stack(), m_cmd.release());
                stack().enableActions();
            }
        }

        void rollback()
        {
            if(m_cmd)
            {
                stack().enableActions();
            }
            m_cmd.reset();
        }

    private:
        std::unique_ptr<iscore::SerializableCommand> m_cmd;
};

namespace Audio
{
namespace Mix
{
class DirectMixTableWidget;
class RoutingTableWidget;
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
        void updateRouting(const Routing&);
        void updateDirectMix(const DirectMix&);

        QTableWidget* m_table{};
        QuietOngoingCommandDispatcher m_dispatcher;

        std::unordered_map<Routing, RoutingTableWidget*> m_routings;
        std::unordered_map<DirectMix, DirectMixTableWidget*> m_directs;
};
}
}
