#include "LV2EffectModel.hpp"
#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>
#include <Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.h>

namespace Audio
{
namespace Effect
{

LV2EffectModel::LV2EffectModel(
        const QString& path,
        const Id<EffectModel>& id,
        QObject* parent):
    EffectModel{id, parent}
{
    init();
}

LV2EffectModel::LV2EffectModel(
        const LV2EffectModel& source,
        const Id<EffectModel>& id,
        QObject* parent):
    EffectModel{id, parent}
{
    init();
}

void LV2EffectModel::init()
{
  auto& world = iscore::AppContext().components.applicationPlugin<Audio::AudioStreamEngine::ApplicationPlugin>().lilv;

  auto plugs = world.get_all_plugins();
  auto it = plugs.begin();
  while(!plugs.is_end(it))
  {

      auto plug = plugs.get(it);
      if(std::string(plug.get_name().as_string()) == "GxFlanger")
      {
          m_effect = MakeLV2AudioEffect(plug.me, world.me);

          return;

      }
      else
      {
          it = plugs.next(it);
      }

  }

}

void LV2EffectModel::reload()
{

}

}
}
