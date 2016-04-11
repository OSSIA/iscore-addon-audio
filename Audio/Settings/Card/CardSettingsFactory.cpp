#include "CardSettingsFactory.hpp"
#include "CardSettingsModel.hpp"
#include "CardSettingsView.hpp"
#include "CardSettingsPresenter.hpp"
#include <iscore/application/ApplicationContext.hpp>
#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>
namespace Audio
{
namespace Settings
{

iscore::SettingsDelegateViewInterface* Factory::makeView() {
    auto& p = iscore::AppContext().components.applicationPlugin<Audio::AudioStreamEngine::ApplicationPlugin>();
    return new View(p);
}

iscore::SettingsDelegatePresenterInterface* Factory::makePresenter_impl(
        iscore::SettingsDelegateModelInterface& m,
        iscore::SettingsDelegateViewInterface& v,
        QObject* parent)
{
    return new Presenter((Model&)m, (View&)v, parent);
}

iscore::SettingsDelegateModelInterface* Factory::makeModel()
{
    return new Model();
}


}
}
