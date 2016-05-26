#pragma once
#include <Audio/Commands/AudioCommandFactory.hpp>
#include <iscore/command/SerializableCommand.hpp>
#include <Audio/Panel/Track.hpp>
#include <Audio/Panel/TrackModel.hpp>
#include <iscore/tools/TreePath.hpp>

namespace Audio {
namespace Commands {

class SetData : public iscore::SerializableCommand {
    ISCORE_COMMAND_DECL(CommandFactoryName(), SetData, "Set data")
public:
    SetData(Path<Panel::TrackModel> device_tree,
            int index,
            QVariant value,
            Panel::TrackModel::TrackRoles role);
    void undo() const override;
    void redo() const override;

protected:
    void serializeImpl(DataStreamInput&) const override;
    void deserializeImpl(DataStreamOutput&) override;

private:
    Path<Panel::TrackModel> m_devicesModel;
    int m_index;
    QVariant m_oldval;
    QVariant m_newval;
    Panel::TrackModel::TrackRoles m_role;
};

}
}
