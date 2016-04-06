#pragma once
#include <iscore/plugins/application/GUIApplicationContextPlugin.hpp>

namespace Audio
{
class ApplicationPlugin : public QObject, public iscore::GUIApplicationContextPlugin
{
    public:
        ApplicationPlugin(const iscore::ApplicationContext& app);

        void on_newDocument(iscore::Document* doc) override;

};
}
