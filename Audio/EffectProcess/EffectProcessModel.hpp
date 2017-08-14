#pragma once
#include <Process/Process.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/serialization/VisitorCommon.hpp>
#include <Audio/EffectProcess/EffectProcessMetadata.hpp>
#include <iscore/model/EntityMap.hpp>
#include <Audio/EffectProcess/Effect/EffectModel.hpp>
namespace Audio
{
namespace Effect
{
class ProcessModel;
/**
 * @brief The Audio::Effect::ProcessModel class
 *
 * This class represents an effect chain.
 * Each effect should provide a component that will create
 * the corresponding LibAudioStream effect.
 *
 * Chaining two effects blocks [A] -> [B] is akin to
 * doing :
 *
 * MakeEffectSound(MakeEffectSound(Original sound, A, 0, 0), B, 0, 0)
 *
 */
class ProcessModel final : public Process::ProcessModel
{
        ISCORE_SERIALIZE_FRIENDS
        PROCESS_METADATA_IMPL(Audio::Effect::ProcessModel)

        Q_OBJECT
    public:
        explicit ProcessModel(
                const TimeVal& duration,
                const Id<Process::ProcessModel>& id,
                QObject* parent);

        explicit ProcessModel(
                const ProcessModel& source,
                const Id<Process::ProcessModel>& id,
                QObject* parent);

        virtual ~ProcessModel();

        template<typename Impl>
        explicit ProcessModel(
                Impl& vis,
                QObject* parent) :
            Process::ProcessModel{vis, parent}
        {
            vis.writeTo(*this);
        }

        const iscore::EntityMap<EffectModel>& effects() const
        { return m_effects; }
        const auto& effectsOrder() const
        { return m_effectOrder; }

        void insertEffect(EffectModel* eff, int pos);
        void removeEffect(const Id<EffectModel>&);
        void moveEffect(const Id<EffectModel>&, int new_pos);

        int effectPosition(const Id<EffectModel>& e) const;

    signals:
        void effectsChanged();

    private:
        // The actual effect instances
        iscore::EntityMap<EffectModel> m_effects;

        // The effect chain.
        QList<Id<EffectModel>> m_effectOrder;
};
}
}
