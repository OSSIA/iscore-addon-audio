#pragma once
#include <Scenario/Application/Drops/ScenarioDropHandler.hpp>
namespace Audio
{
namespace Sound
{
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
};
}
}
