#pragma once
#include <Process/LayerView.hpp>
#include <Audio/AudioArray.hpp>
#include <Process/TimeValue.hpp>
#include <Audio/MediaFileHandle.hpp>
#include <Process/ZoomHelper.hpp>

namespace Audio
{
namespace Sound
{
class LayerView final : public Process::LayerView
{
        Q_OBJECT
    public:
        explicit LayerView(QGraphicsItem* parent);

        void setData(const MediaFileHandle& data);
        void recompute(const TimeValue& dur, ZoomRatio ratio);
    signals:
        void pressed();

    private:
        void paint_impl(QPainter*) const override;
        void mousePressEvent(QGraphicsSceneMouseEvent*) override;

        AudioArray m_data{};
        QPainterPath m_path;
        int m_sampleRate;
};
}
}
