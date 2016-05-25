#pragma once
#include <Audio/Commands/AudioCommandFactory.hpp>
#include <iscore/command/SerializableCommand.hpp>
#include <Audio/Panel/Track.hpp>
#include <iscore/tools/TreePath.hpp>

namespace Audio {
namespace Panel {
    class TrackModel;
}

namespace Commands {


class AddTrack : public iscore::SerializableCommand {
    ISCORE_COMMAND_DECL(CommandFactoryName(), AddTrack, "Add a track")
public:
    AddTrack(Path<Panel::TrackModel> device_tree, const Panel::Track& t);
    void undo() const override;
    void redo() const override;

    int createdTrackIndex() const;

protected:
    void serializeImpl(DataStreamInput&) const override;
    void deserializeImpl(DataStreamOutput&) override;

private:
    Path<Panel::TrackModel> m_devicesModel;
    int m_index;
    Panel::Track m_track;
};


}
}
