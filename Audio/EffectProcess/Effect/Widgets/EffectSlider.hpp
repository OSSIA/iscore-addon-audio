#pragma once
#include <QWidget>
#include <State/Address.hpp>
#include <ossia/network/base/address.hpp>
namespace ossia
{
namespace net
{
class node_base;
}
}
namespace iscore
{
class DoubleSlider;
}

namespace Audio
{
namespace Effect
{
class EffectSlider : public QWidget
{
    Q_OBJECT
    public:
        EffectSlider(const ossia::net::node_base& fx, bool is_output, QWidget* parent);

        ~EffectSlider();

        double scaledValue;
        iscore::DoubleSlider* m_slider{};
    signals:
        void createAutomation(const State::Address&, double min, double max);

    private:
        void contextMenuEvent(QContextMenuEvent* event) override;

        const ossia::net::node_base& m_param;
        ossia::net::address_base::callback_index m_callback;
        float m_min{0.};
        float m_max{1.};

        QAction* m_addAutomAction{};
};
}
}
