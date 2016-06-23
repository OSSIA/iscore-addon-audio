#include "SoundDrop.hpp"
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


#include <Loop/LoopProcessModel.hpp>
#include <QMimeData>
#include <QUrl>
#include <QApplication>
#include <Scenario/Commands/MoveBaseEvent.hpp>
namespace Audio
{
namespace Sound
{
using disp = GenericMacroCommandDispatcher<RedoStrategy::Redo, SendStrategy::Quiet>;

static void createSoundProcesses(
        disp& m,
        const Scenario::ConstraintModel& constraint,
        const Scenario::RackModel& rack,
        DroppedAudioFiles& drop)
{
    for(auto&& file : drop.files)
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
}

DroppedAudioFiles::DroppedAudioFiles(const QMimeData &mime)
{
    for(auto url : mime.urls())
    {
        QString filename = url.toLocalFile();
        if(!MediaFileHandle::isAudioFile(QFile{filename}))
            continue;

        files.emplace_back(filename);

        // TODO what if 0 channels
        auto samples = files.back().samples();
        if(samples > maxDuration)
        {
            maxDuration = samples;
            maxSampleRate = files.back().sampleRate();
        }
    }
}

TimeValue DroppedAudioFiles::dropMaxDuration() const
{
    // TODO what about resampling.
    return TimeValue::fromMsecs(maxDuration / (maxSampleRate / 1000));
}


bool DropHandler::handle(
        const Scenario::TemporalScenarioPresenter& pres,
        QPointF pos,
        const QMimeData *mime)
{
    DroppedAudioFiles drop{*mime};
    if(!drop.valid())
    {
        return false;
    }

    // We add the files in sequence, or in the same constraint
    // if shift isn't pressed.
    if(qApp->keyboardModifiers() & Qt::ShiftModifier)
    {
        return createInSequence(pres, pos, std::move(drop));
    }
    else
    {
        return createInParallel(pres, pos, std::move(drop));
    }
}

bool DropHandler::createInSequence(
        const Scenario::TemporalScenarioPresenter& pres,
        QPointF pos,
        DroppedAudioFiles&& audio)
{
    ISCORE_TODO;
    return true;
}

bool DropHandler::createInParallel(
        const Scenario::TemporalScenarioPresenter& pres,
        QPointF pos,
        DroppedAudioFiles&& drop)
{
    disp m{new Audio::Commands::CreateSoundBoxMacro,
           pres.context().context.commandStack};

    // Create a box.
    const Scenario::ScenarioModel& scenar = pres.processModel();
    Scenario::Point pt = pres.toScenarioPoint(pos);

    TimeValue t = drop.dropMaxDuration();

    // Create the beginning
    auto start_cmd = new Scenario::Command::CreateTimeNode_Event_State{scenar, pt.date, pt.y};
    m.submitCommand(start_cmd);

    // Create a box with the duration of the longest song
    auto box_cmd = new Scenario::Command::CreateConstraint_State_Event_TimeNode{
            scenar, start_cmd->createdState(), pt.date + t, pt.y};
    m.submitCommand(box_cmd);
    auto& constraint = scenar.constraint(box_cmd->createdConstraint());

    auto cmd3 = new Scenario::Command::AddRackToConstraint{constraint};
    m.submitCommand(cmd3);

    auto& rack = constraint.racks.at(cmd3->createdRack());

    // Add sound processes as fit.
    createSoundProcesses(m, constraint, rack, drop);

    // Finally we show the newly created rack
    auto show_cmd = new Scenario::Command::ShowRackInAllViewModels{constraint, rack.id()};
    m.submitCommand(show_cmd);
    m.commit();

    return true;
}

// TODO put me in some "algorithms" file.
static bool constraintHasNoFollowers(
        const Scenario::ScenarioModel& scenar,
        const Scenario::ConstraintModel& cst)
{
    auto& tn = Scenario::endTimeNode(cst, scenar);
    for(auto& event_id : tn.events())
    {
        Scenario::EventModel& event = scenar.events.at(event_id);
        for(auto& state_id : event.states())
        {
            Scenario::StateModel& state = scenar.states.at(state_id);
            if(state.nextConstraint())
                return false;
        }
    }
    return true;
}


bool ConstraintDropHandler::handle(
        const Scenario::ConstraintModel& constraint,
        const QMimeData *mime)
{
    DroppedAudioFiles drop{*mime};
    if(!drop.valid())
    {
        return false;
    }

    using disp = GenericMacroCommandDispatcher<RedoStrategy::Redo, SendStrategy::Quiet>;
    auto& doc = iscore::IDocument::documentContext(constraint);
    disp m{new Audio::Commands::CreateSoundBoxMacro, doc.commandStack};

    // TODO dynamic_safe_cast ? for non-static-castable types, have the compiler enforce dynamic_cast ?
    auto scenar = dynamic_cast<Scenario::ScenarioInterface*>(constraint.parent());
    ISCORE_ASSERT(scenar);

    // If the constraint has no processes and nothing after, we will resize it
    if(constraint.processes.empty())
    {
        // TODO refactor me in a general "move event in scenario interface" way.
        if(auto loop = dynamic_cast<Loop::ProcessModel*>(scenar))
        {
            auto resize_cmd = new Scenario::Command::MoveBaseEvent<Loop::ProcessModel>{
                            *loop,
                            loop->endEvent().id(),
                            drop.dropMaxDuration(),
                            constraint.heightPercentage(),
                            ExpandMode::GrowShrink};
            m.submitCommand(resize_cmd);
        }
        else if(auto scenar = dynamic_cast<Scenario::ScenarioModel*>(constraint.parent()))
        {
            // First check that the end time node has nothing afterwards :
            // all its states must not have next constraints
            if(constraintHasNoFollowers(*scenar, constraint))
            {
                auto& ev = Scenario::endState(constraint, *scenar).eventId();
                auto resize_cmd = new Scenario::Command::MoveEventMeta{
                        *scenar,
                        ev,
                        drop.dropMaxDuration(),
                        constraint.heightPercentage(),
                        ExpandMode::GrowShrink};
                m.submitCommand(resize_cmd);

            }
        }
    }

    auto& rack = [&] () -> Scenario::RackModel& {
        if(constraint.racks.empty())
        {
            auto rack_cmd = new Scenario::Command::AddRackToConstraint{constraint};
            m.submitCommand(rack_cmd);
            return constraint.racks.at(rack_cmd->createdRack());
        }
        else
        {
            return *constraint.racks.begin();
        }
    }();


    // Add the processes
    createSoundProcesses(m, constraint, rack, drop);

    // Show the rack
    auto show_cmd = new Scenario::Command::ShowRackInAllViewModels{constraint, rack.id()};
    m.submitCommand(show_cmd);
    m.commit();

    return false;
}

}
}
