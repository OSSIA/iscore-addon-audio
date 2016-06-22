#include "SoundDrop.hpp"
#include <Audio/MediaFileHandle.hpp>
#include <Audio/Commands/ChangeAudioFile.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>

#include <Scenario/Process/Temporal/TemporalScenarioPresenter.hpp>
#include <Scenario/Process/Temporal/TemporalScenarioView.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Scenario/Commands/Constraint/AddProcessToConstraint.hpp>
#include <iscore/command/Dispatchers/MacroCommandDispatcher.hpp>
#include <Scenario/Commands/Scenario/Creations/CreateTimeNode_Event_State.hpp>

#include <QMimeData>
#include <QUrl>
#include <QApplication>
namespace Audio
{
namespace Sound
{

class CreateSoundBoxMacro final : public iscore::AggregateCommand
{
        ISCORE_COMMAND_DECL(Audio::CommandFactoryName(), CreateSoundBoxMacro, "Add sounds in a box")
};

class CreateSoundBoxesMacro final : public iscore::AggregateCommand
{
        ISCORE_COMMAND_DECL(Audio::CommandFactoryName(), CreateSoundBoxesMacro, "Add sounds in sequence")
};

bool DropHandler::handle(
        const Scenario::TemporalScenarioPresenter& pres,
        QPointF pos,
        const QMimeData *mime)
{
    if(!mime->hasUrls())
        return false;

    using disp = GenericMacroCommandDispatcher<RedoStrategy::Redo, SendStrategy::Quiet>;
    // We add the files in sequence, or in the same constraint
    // if shift is pressed.
    if(qApp->keyboardModifiers() & Qt::ShiftModifier)
    {
        disp m(
                    new CreateSoundBoxesMacro,
                    pres.context().context.commandStack);
    }
    else
    {
        disp m(
                    new CreateSoundBoxMacro,
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


        // Add sound processes as fit.
        for(auto&& file : files)
        {
            auto process_cmd = Scenario::Command::make_AddProcessToConstraint(
                        constraint,
                        Metadata<ConcreteFactoryKey_k, Audio::Sound::ProcessModel>::get());
            m.submitCommand(process_cmd);

            auto& proc = safe_cast<Sound::ProcessModel&>(constraint.processes.at(process_cmd->processId()));


            auto file_cmd = new Audio::Commands::ChangeAudioFile{proc, std::move(file)};
            m.submitCommand(file_cmd);
        }

        m.commit();
    }


    return true;
}

}
}
