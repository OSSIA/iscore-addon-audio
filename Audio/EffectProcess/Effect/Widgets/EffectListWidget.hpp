#pragma once
#include <QWidget>
#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
#include <nano_observer.hpp>
#include <iscore/tools/ModelPath.hpp>
namespace Scenario
{
}
namespace iscore
{
struct DocumentContext;
}
class QHBoxLayout;
namespace Audio
{
namespace Effect
{
class EffectWidget;
class ProcessModel;
class EffectModel;
class EffectListWidget :
        public QWidget,
        public Nano::Observer
{
        Q_OBJECT
    public:
        EffectListWidget(
                const Effect::ProcessModel& fx,
                const iscore::DocumentContext& doc,
                QWidget* parent);

        void on_effectRemoved(const EffectModel& fx);

        void setup();

    signals:
        void pressed();

    private:
        void mousePressEvent(QMouseEvent* event) override;
        void dragEnterEvent(QDragEnterEvent* event) override;
        void dragMoveEvent(QDragMoveEvent* event) override;
        void dropEvent(QDropEvent *event) override;

        const Effect::ProcessModel& m_effects;
        const iscore::DocumentContext& m_context;
        CommandDispatcher<> m_dispatcher;

        QHBoxLayout* m_layout{};

        std::vector<EffectWidget*> m_widgets;
};
}
}
