#pragma once
#include <score/plugins/settingsdelegate/SettingsDelegatePresenter.hpp>

namespace Audio {
namespace Settings {

class Model;
class View;
class Presenter : public score::SettingsDelegatePresenter {
public:
    using model_type = Model;
    using view_type = View;
    Presenter(Model&, View&, QObject* parent);

private:
    QString settingsName() override;
    QIcon settingsIcon() override;
};

}
}
