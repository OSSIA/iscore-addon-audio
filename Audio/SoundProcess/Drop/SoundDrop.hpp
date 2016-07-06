#pragma once
#include <Scenario/Application/Drops/ScenarioDropHandler.hpp>
#include <Audio/MediaFileHandle.hpp>
#include <Process/TimeValue.hpp>
namespace Audio
{
namespace Sound
{
struct DroppedAudioFiles
{
        DroppedAudioFiles(const QMimeData& mime);

        bool valid() const
        { return !files.empty() && maxDuration != 0; }

        TimeValue dropMaxDuration() const;
        double maxDuration = 0;
        double maxSampleRate = 0;
        std::vector<MediaFileHandle> files;
};

/**
 * @brief The DropHandler class
 * If something with audio mime type is dropped,
 * then we create a box with an audio file loaded.
 */
class DropHandler final :
        public Scenario::DropHandler
{
        ISCORE_CONCRETE_FACTORY_DECL("bc57983b-c29e-4b12-8afe-9d6ffbcb7a94")

        bool handle(
                 const Scenario::TemporalScenarioPresenter &,
                 QPointF pos,
                 const QMimeData *mime) override;

        bool createInSequence(
                const Scenario::TemporalScenarioPresenter &,
                QPointF pos,
                DroppedAudioFiles&& audio);
        bool createInParallel(
                const Scenario::TemporalScenarioPresenter &,
                QPointF pos,
                DroppedAudioFiles&& audio);
};


class ConstraintDropHandler final :
        public Scenario::ConstraintDropHandler
{
        ISCORE_CONCRETE_FACTORY_DECL("edbc884b-96cc-4b59-998c-2f48941a7b6a")

        bool handle(
                 const Scenario::ConstraintModel&,
                 const QMimeData *mime) override;
};

// TODO drop in slots, drop in rack ?
}
}
