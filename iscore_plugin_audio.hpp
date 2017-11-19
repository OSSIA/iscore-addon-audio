#pragma once
#include <QObject>
#include <score/plugins/qt_interfaces/PluginRequirements_QtInterface.hpp>
#include <score/plugins/qt_interfaces/FactoryInterface_QtInterface.hpp>
#include <score/plugins/qt_interfaces/FactoryFamily_QtInterface.hpp>
#include <score/plugins/qt_interfaces/GUIApplicationPlugin_QtInterface.hpp>
#include <score/plugins/qt_interfaces/CommandFactory_QtInterface.hpp>

class SCORE_plugin_audio final:
        public QObject,
        public score::Plugin_QtInterface,
        public score::ApplicationPlugin_QtInterface,
        public score::FactoryInterface_QtInterface,
        public score::FactoryList_QtInterface,
        public score::CommandFactory_QtInterface
{
        Q_OBJECT
        Q_PLUGIN_METADATA(IID FactoryInterface_QtInterface_iid)
        Q_INTERFACES(
                score::Plugin_QtInterface
                score::ApplicationPlugin_QtInterface
                score::FactoryInterface_QtInterface
                score::FactoryList_QtInterface
                score::CommandFactory_QtInterface
                )
  SCORE_PLUGIN_METADATA(1, "f07abe79-1b83-4abd-b002-958c878755c1")

    public:
        SCORE_plugin_audio();
        ~SCORE_plugin_audio();

        score::ApplicationPlugin* make_applicationPlugin(
                const score::ApplicationContext& app) override;
        score::GUIApplicationPlugin* make_guiApplicationPlugin(
                const score::GUIApplicationContext& app) override;


        std::vector<std::unique_ptr<score::InterfaceListBase>> factoryFamilies() override;

        std::vector<std::unique_ptr<score::InterfaceBase>> factories(
                const score::ApplicationContext& ctx,
                const score::InterfaceKey& factoryName) const override;

        // CommandFactory_QtInterface interface
        std::pair<const CommandGroupKey, CommandGeneratorMap> make_commands() override;

};
