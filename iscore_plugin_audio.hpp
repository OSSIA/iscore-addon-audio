#pragma once
#include <QObject>
#include <iscore/plugins/qt_interfaces/PluginRequirements_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/FactoryInterface_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/FactoryFamily_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/GUIApplicationContextPlugin_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/CommandFactory_QtInterface.hpp>

class iscore_plugin_audio final:
        public QObject,
        public iscore::Plugin_QtInterface,
        public iscore::GUIApplicationContextPlugin_QtInterface,
        public iscore::FactoryInterface_QtInterface,
        public iscore::FactoryList_QtInterface,
        public iscore::CommandFactory_QtInterface
{
        Q_OBJECT
        Q_PLUGIN_METADATA(IID FactoryInterface_QtInterface_iid)
        Q_INTERFACES(
                iscore::Plugin_QtInterface
                iscore::GUIApplicationContextPlugin_QtInterface
                iscore::FactoryInterface_QtInterface
                iscore::FactoryList_QtInterface
                iscore::CommandFactory_QtInterface
                )
  ISCORE_PLUGIN_METADATA(1, "f07abe79-1b83-4abd-b002-958c878755c1")

    public:
        iscore_plugin_audio();
        ~iscore_plugin_audio();

        iscore::GUIApplicationContextPlugin* make_applicationPlugin(
                const iscore::GUIApplicationContext& app) override;


        std::vector<std::unique_ptr<iscore::InterfaceListBase>> factoryFamilies() override;

        std::vector<std::unique_ptr<iscore::InterfaceBase>> factories(
                const iscore::ApplicationContext& ctx,
                const iscore::InterfaceKey& factoryName) const override;

        // CommandFactory_QtInterface interface
        std::pair<const CommandGroupKey, CommandGeneratorMap> make_commands() override;

};
