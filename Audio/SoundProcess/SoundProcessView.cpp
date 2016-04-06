#include "SoundProcessView.hpp"
namespace Audio
{
namespace SoundProcess
{

LayerView::LayerView(QGraphicsItem* parent):
    Process::LayerView{parent}
{
}

void LayerView::setData(const MediaFileHandle& data)
{
    m_data = data.data();
}

void LayerView::recompute(const TimeValue& dur, ZoomRatio ratio)
{
    m_path = QPainterPath{};
    m_path.setFillRule(Qt::WindingFill);

    if(m_data.size() == 0)
        return;

    const auto& chan = m_data[0];
    if(chan.size() < 1000)
        return;

    // Compute 1 point every 10 pixels

    // Get how much data point is 10 pixels :
    // ratio is milliseconds / pixels
    // dur is milliseconds corresponding to width()
    // rate is 44100
    // rate.rate.
    // TODO use real file rate
    // TODO > 1 channels

    // Each "density" pixels we want to take a new data point
    const int density = 10;

    // The duration of 10 pixels is 10 * ratio
    const double interval_duration = density * ratio;

    const int samples_in_interval = interval_duration * 44100 / 1000;

    const auto h = height();
    const auto w = width();
    const auto chan_n = chan.size();
    const int n_samples = std::max(int(w / density), int(chan_n / density));
    const int n_mult = 1;

    m_path.moveTo(0, chan[0] * n_mult * h + h / 2.);
    for(int i = 1; i < n_samples - 1 && i * samples_in_interval < chan_n; i ++)
    {
        m_path.lineTo(
                    i * density,
                    chan[i * samples_in_interval] * n_mult * h + h / 2.);
    }

    update();
}

void LayerView::paint_impl(QPainter* painter) const
{
    painter->setBrush(Qt::darkCyan);
    painter->setPen(Qt::darkBlue);

    painter->drawPath(m_path);
}

void LayerView::mousePressEvent(QGraphicsSceneMouseEvent*)
{
    emit pressed();
}
}
}
