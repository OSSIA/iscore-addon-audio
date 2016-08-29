#pragma once
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <State/Address.hpp>
#include <Audio/EffectProcess/Effect/EffectModel.hpp>
#include <iscore/widgets/ClearLayout.hpp>
#include <iscore/widgets/ReactiveLabel.hpp>

namespace Audio
{
namespace Effect
{
class EffectSlider;
/**
 * @brief The EffectWidget class
 *
 * Controls for a single effect.
 * Show a list of sliders for each declared parameter.
 */
class EffectWidget :
        public QFrame,
        public Nano::Observer
{
        Q_OBJECT
    public:
        EffectWidget(
                EffectModel& fx,
                const iscore::DocumentContext& doc,
                QWidget* parent);

        auto& effect() const { return m_effect; }

    signals:
        void removeRequested();
        void sig_performInsert(
                const Path<Effect::EffectModel>& fx1,
                const Path<Effect::EffectModel>& fx2,
                bool after);

    private:
        void mousePressEvent(QMouseEvent * event) override;
        void dragEnterEvent(QDragEnterEvent* event) override;
        void dragMoveEvent(QDragMoveEvent* event) override;
        void dropEvent(QDropEvent* event) override;


        void on_createAutomation(const State::Address&, double min, double max);

        void setup();

        void resizeEvent(QResizeEvent *event) override;

        void flow();

        void reflow();

        void componentAdded(const iscore::Component &);
        EffectModel& m_effect;
        const iscore::DocumentContext& m_context;
        QGridLayout* m_layout{};
        std::vector<EffectSlider*> m_sliders;
};
}
}
Q_DECLARE_METATYPE(Path<Audio::Effect::EffectModel>)
