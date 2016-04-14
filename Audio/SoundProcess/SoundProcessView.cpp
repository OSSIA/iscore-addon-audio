#include "SoundProcessView.hpp"
#include <QPainter>

namespace Audio
{
namespace Sound
{

LayerView::LayerView(QGraphicsItem* parent):
    Process::LayerView{parent}
{
}

void LayerView::setData(const MediaFileHandle& data)
{
    m_data = data.data();
    m_sampleRate = data.sampleRate();
}

void LayerView::recompute(const TimeValue& dur, ZoomRatio ratio)
{
    m_path = QPainterPath{};
    m_path.setFillRule(Qt::WindingFill);

    int nchannels = m_data.size();

    if(nchannels == 0)
        return;

    // height of each channel
    const auto h = height() / (double)nchannels;

    const int w = width();

    // Trace lines between channels
    // TODO change color for those lines

    for (int c = 1 ; c < nchannels; ++c) {
        m_path.moveTo(0, c * h);
        m_path.lineTo(w, c * h);
    }

    const int density = 10;

    // Compute 1 point every <density> pixels

    // Get how much data point is <density> pixels :
    // ratio is milliseconds / pixels
    // dur is milliseconds corresponding to width()

    // The duration of n pixels is n * ratio

    const double interval_duration = density * ratio;

    const int samples_in_interval = interval_duration * m_sampleRate / 1000.;

    for (int c = 0; c < nchannels; ++c) {

        const auto& chan = m_data[c];
        const auto chan_n = chan.size();
        const int n_samples = std::max(int(w / density), int(chan_n / density));
        const int current_height = c * h;

        m_path.moveTo(0, chan[0] + current_height + h / 2.);

//        for(int i = 1; i < n_samples - 1 && i * samples_in_interval < chan_n; i ++)
//        {
//            m_path.lineTo(
//                        i * density,
//                        chan[i * samples_in_interval] * n_mult * h + h / 2.);
//        }

        for (int i = 0 ; i < n_samples && i * samples_in_interval < chan_n - density; ++i) {
            double rms = 0;
            for (int j = 0; j < density; ++j) {
                auto s = chan[i * samples_in_interval + j];
                rms += s * s;
            }
            rms = -std::sqrt(rms) / density;
            m_path.lineTo(i * density, rms * h + current_height + h/2.);
        }

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
