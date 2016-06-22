#include "SoundDrop.hpp"
#include <Audio/MediaFileHandle.hpp>
#include <Audio/Commands/ChangeAudioFile.hpp>
#include <Audio/Commands/CreateSoundBox.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>

#include <Scenario/Process/Temporal/TemporalScenarioPresenter.hpp>
#include <Scenario/Process/Temporal/TemporalScenarioView.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Scenario/Commands/Constraint/AddProcessToConstraint.hpp>
#include <iscore/command/Dispatchers/MacroCommandDispatcher.hpp>
#include <Scenario/Commands/Scenario/Creations/CreateTimeNode_Event_State.hpp>
#include <Scenario/Commands/Constraint/AddRackToConstraint.hpp>
#include <Scenario/Commands/Constraint/Rack/AddSlotToRack.hpp>
#include <Scenario/Commands/Constraint/AddLayerInNewSlot.hpp>
#include <Scenario/Commands/Constraint/Rack/Slot/AddLayerModelToSlot.hpp>


#include <QMimeData>
#include <QUrl>
#include <QApplication>
namespace Audio
{
namespace Sound
{

bool DropHandler::handle(
        const Scenario::TemporalScenarioPresenter& pres,
        QPointF pos,
        const QMimeData *mime)
{
    if(!mime->hasUrls())
        return false;

    using disp = GenericMacroCommandDispatcher<RedoStrategy::Redo, SendStrategy::Quiet>;
    // We add the files in sequence, or in the same constraint
    // if shift isn't pressed.
    if(qApp->keyboardModifiers() & Qt::ShiftModifier)
    {
        ISCORE_TODO;
        return false;
    }
    else
    {
        disp m(
                    new Audio::Commands::CreateSoundBoxMacro,
                    pres.context().context.commandStack);

        // Create a box.
        const Scenario::ScenarioModel& scenar = pres.processModel();
        Scenario::Point pt = pres.toScenarioPoint(pos);

        int64_t maxDuration = 0;
        std::vector<MediaFileHandle> files;
        for(auto url : mime->urls())
        {
            QString filename = url.toLocalFile();
            if(!MediaFileHandle::isAudioFile(QFile{filename}))
                continue;

            files.emplace_back(filename);

            maxDuration = std::max(maxDuration, files.back().samples());
        }

        if(files.empty() || maxDuration == 0)
        {
            m.rollback();
            return true;
        }

        // TODO fetch from settings
        TimeValue t = TimeValue::fromMsecs(maxDuration / 44.100);
        auto cmd1 = new Scenario::Command::CreateTimeNode_Event_State{scenar, pt.date, pt.y};
        m.submitCommand(cmd1);
        auto cmd2 = new Scenario::Command::CreateConstraint_State_Event_TimeNode{
                scenar, cmd1->createdState(), pt.date + t, pt.y};
        m.submitCommand(cmd2);
        auto& constraint = scenar.constraint(cmd2->createdConstraint());

        auto cmd3 = new Scenario::Command::AddRackToConstraint{constraint};
        m.submitCommand(cmd3);

        auto& rack = constraint.racks.at(cmd3->createdRack());

        // Add sound processes as fit.
        for(auto&& file : files)
        {
            // Create sound process
            auto process_cmd = new Scenario::Command::AddOnlyProcessToConstraint{
                        constraint,
                        Metadata<ConcreteFactoryKey_k, Audio::Sound::ProcessModel>::get()};
            m.submitCommand(process_cmd);

            // Set process file
            auto& proc = safe_cast<Sound::ProcessModel&>(constraint.processes.at(process_cmd->processId()));
            auto file_cmd = new Audio::Commands::ChangeAudioFile{proc, std::move(file)};
            m.submitCommand(file_cmd);

            // Create a new slot
            auto slot_cmd = new Scenario::Command::AddSlotToRack{rack};
            m.submitCommand(slot_cmd);

            // Add a new layer in this slot.
            auto& slot = rack.slotmodels.at(slot_cmd->createdSlot());

            auto layer_cmd = new Scenario::Command::AddLayerModelToSlot{slot, proc};
            m.submitCommand(layer_cmd);
        }

        // Finally we show the newly created rack
        auto show_cmd = new Scenario::Command::ShowRackInAllViewModels{constraint, rack.id()};
        m.submitCommand(show_cmd);
        m.commit();
    }


    return true;
}

}
}
