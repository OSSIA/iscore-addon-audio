#include "CardSettingsFactory.hpp"
#include "CardSettingsModel.hpp"
#include "CardSettingsView.hpp"
#include "CardSettingsPresenter.hpp"
namespace Audio
{
namespace Settings
{

iscore::SettingsDelegateViewInterface* Factory::makeView()
{
    ISCORE_TODO;
    return nullptr;
}

iscore::SettingsDelegatePresenterInterface* Factory::makePresenter_impl(
        iscore::SettingsDelegateModelInterface& m,
        iscore::SettingsDelegateViewInterface& v,
        QObject* parent)
{
    ISCORE_TODO;
    return nullptr;
}

iscore::SettingsDelegateModelInterface* Factory::makeModel()
{
    return new Model;
}


}
}
