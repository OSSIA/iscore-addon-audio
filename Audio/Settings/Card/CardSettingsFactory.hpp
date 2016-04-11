#pragma once
#include <iscore/plugins/settingsdelegate/SettingsDelegateFactoryInterface.hpp>

namespace Audio
{
namespace Settings
{

class Factory : public iscore::SettingsDelegateFactory
{
        ISCORE_CONCRETE_FACTORY_DECL("e7c0bd6d-a6e2-412f-844f-125a2ad07394")

        iscore::SettingsDelegateViewInterface *makeView() override;
        iscore::SettingsDelegatePresenterInterface* makePresenter_impl(
                iscore::SettingsDelegateModelInterface& m,
                iscore::SettingsDelegateViewInterface& v,
                QObject* parent) override;
        iscore::SettingsDelegateModelInterface *makeModel() override;
};

}
}
