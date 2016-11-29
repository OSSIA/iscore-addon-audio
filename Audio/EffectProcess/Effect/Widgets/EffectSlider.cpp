#include "EffectSlider.hpp"
#include <ossia/network/domain/domain.hpp>
#include <State/Expression.hpp>
#include <iscore/widgets/DoubleSlider.hpp>
#include <iscore/widgets/MarginLess.hpp>
#include <Device/Node/NodeListMimeSerialization.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/editor/value/value_conversion.hpp>
#include <Engine/OSSIA2iscore.hpp>
#include <QMenu>
#include <QLabel>
#include <QDrag>
#include <QVBoxLayout>
#include <QAction>
#include <QContextMenuEvent>
#include <Device/Address/AddressSettings.hpp>
#include <State/MessageListSerialization.hpp>

namespace Audio
{
namespace Effect
{
// TODO move me
class AddressLabel : public QLabel
{
        const ossia::net::node_base& m_node;
    public:
        AddressLabel(const ossia::net::node_base& node, QString str, QWidget* parent):
            QLabel{std::move(str), parent},
            m_node{node}
        {

        }

    private:
        void mousePressEvent(QMouseEvent* event) override
        {
            if (event->button() == Qt::LeftButton)
            {
                auto as = Engine::ossia_to_iscore::ToFullAddressSettings(m_node);

                auto drag = new QDrag(this);
                auto mimeData = new QMimeData;
                {
                    Mime<Device::FullAddressSettings>::Serializer s{*mimeData};
                    s.serialize(as);
                }
                {
                    Mime<State::MessageList>::Serializer s{*mimeData};
                    s.serialize({State::Message{State::AddressAccessor{as.address}, as.value}});
                }
                drag->setMimeData(mimeData);

                drag->setPixmap(grab());
                drag->setHotSpot(rect().center());

                drag->exec();
            }
        }

};

EffectSlider::EffectSlider(const ossia::net::node_base& fx, bool is_output, QWidget* parent):
  QWidget{parent},
  m_param{fx}
{
  m_param.aboutToBeDeleted.connect<EffectSlider, &EffectSlider::on_paramDeleted>(this);

  auto addr = m_param.getAddress();

  auto dom = addr->getDomain();

  if(auto f = dom.maybe_min<float>()) m_min = *f;
  if(auto f = dom.maybe_max<float>()) m_max = *f;

  auto lay = new iscore::MarginLess<QVBoxLayout>;
  lay->addWidget(new AddressLabel{
                     m_param,
                     QString::fromStdString(addr->getDescription()),
                     this});
  m_slider = new iscore::DoubleSlider{this};
  m_slider->setEnabled(!is_output);
  m_slider->setForegroundRole(QPalette::AlternateBase);
  lay->addWidget(m_slider);
  this->setLayout(lay);

  auto cur_val = ossia::convert<float>(m_param.getAddress()->cloneValue());
  scaledValue = (cur_val - m_min) / (m_max - m_min);
  m_slider->setValue(scaledValue);

  if(!is_output)
  {
      connect(m_slider, &iscore::DoubleSlider::valueChanged,
              this, [=] (double v)
      {
          ISCORE_ASSERT(!m_paramIsDead);
          // TODO undo ???
          // v is between 0 - 1
          auto cur = m_param.getAddress()->cloneValue();
          auto exp = float(m_min + (m_max - m_min) * v);
          if(ossia::convert<float>(cur) != exp)
              m_param.getAddress()->pushValue(float{exp});

      });
  }
  m_callback = addr->add_callback([=] (const ossia::value& val)
  {
    if(auto v = val.target<float>())
    {
      scaledValue = (*v - m_min) / (m_max - m_min);
     // if(scaled != m_slider->value()) // TODO qFuzzyCompare instead
     //   m_slider->setValue(scaled);
    }
  });

  m_addAutomAction = new QAction{tr("Add automation"), this};
  connect(m_addAutomAction, &QAction::triggered,
          this, [=] () {
    ISCORE_ASSERT(!m_paramIsDead);
    auto& ossia_addr = *m_param.getAddress();
    auto& dom = ossia_addr.getDomain();
    auto addr = State::parseAddress(QString::fromStdString(ossia::net::address_string_from_node(ossia_addr)));

    if(addr)
    {
      auto min = dom.convert_min<double>();
      auto max = dom.convert_max<double>();
      emit createAutomation(std::move(*addr), min, max);
    }
  });
  // TODO show tooltip with current value
}

EffectSlider::~EffectSlider()
{
    if(!m_paramIsDead)
    {
        if(auto addr = m_param.getAddress())
        {
          addr->remove_callback(m_callback);
        }
    }
}

void EffectSlider::contextMenuEvent(QContextMenuEvent* event)
{
  QMenu* contextMenu = new QMenu{this};

  contextMenu->addAction(m_addAutomAction);
  contextMenu->exec(event->globalPos());

  contextMenu->deleteLater();
}

void EffectSlider::on_paramDeleted(const ossia::net::node_base&)
{
    if(auto addr = m_param.getAddress())
    {
      addr->remove_callback(m_callback);
    }

    m_paramIsDead = true;
}

}
}
