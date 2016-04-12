#pragma once
#include <Audio/EffectProcess/EffectFactory.hpp>
#include <iscore/tools/IdentifiedObject.hpp>
#include <iscore/plugins/customfactory/SerializableInterface.hpp>
#include <iscore_plugin_audio_export.h>

namespace Audio
{
namespace Effect
{
class ISCORE_PLUGIN_AUDIO_EXPORT EffectModel :
        public IdentifiedObject<EffectModel>,
        public iscore::SerializableInterface<EffectFactory>
{
        Q_OBJECT
        ISCORE_SERIALIZE_FRIENDS(Audio::Effect::EffectModel, DataStream)
        ISCORE_SERIALIZE_FRIENDS(Audio::Effect::EffectModel, JSONObject)
    public:
        EffectModel(
                const Id<EffectModel>&,
                QObject* parent);

        EffectModel(
                const EffectModel& source,
                const Id<EffectModel>&,
                QObject* parent);

        template<typename Impl>
        EffectModel(
                Deserializer<Impl>& vis,
                QObject* parent) :
            IdentifiedObject<EffectModel>{vis, parent}
        {
            vis.writeTo(*this);
        }

        virtual ~EffectModel();

        virtual EffectModel* clone(
                const Id<EffectModel>& newId,
                QObject* parent) const = 0;

    private:
};
}
}