#pragma once
#include <score/plugins/settingsdelegate/SettingsDelegateFactory.hpp>

#include <Audio/Settings/Card/CardSettingsModel.hpp>
#include <Audio/Settings/Card/CardSettingsPresenter.hpp>
#include <Audio/Settings/Card/CardSettingsView.hpp>

namespace Audio
{
namespace Settings
{
SCORE_DECLARE_SETTINGS_FACTORY(Factory, Model, Presenter, View, "e7c0bd6d-a6e2-412f-844f-125a2ad07394")
}
}

