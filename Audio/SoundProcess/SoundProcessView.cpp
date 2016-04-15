#include "SoundProcessView.hpp"
#include <QPainter>
#include <iostream>

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
    std::cout << "ratio is " << ratio << std::endl;

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

    // Compute 1 point every <density> pixels

    // Get how much data point is <density> pixels :
    // ratio is milliseconds / pixels
    // dur is milliseconds corresponding to width()

    for (int c = 0; c < nchannels; ++c) {

        const auto& chan = m_data[c];
        const auto chan_n = chan.size();

        const double length = (1000 * chan_n) / m_sampleRate;
        const double size = ratio > 0 ? length / ratio : 0;
        const int density = std::max((int)(ratio * m_sampleRate / 1000), 1);

        const int npoints = std::min(w, (int)size);
        const int current_height = c * h;

        std::vector<double> rmsv;
        double maxrms = 0;

        for (int i = 0; i < npoints; ++i) {
            double rms = 0;
            bool reachedEnd = false;
            for (int j = 0; j < density && !reachedEnd; ++j) {
                reachedEnd = i * density + j >= chan.size();
                auto s = chan[i * density + j];
                rms += s * s;
            }
            rmsv.push_back(std::sqrt(rms / density));

            if (rmsv[i] > maxrms)
                maxrms = rmsv[i];
        }

        double hcoeff = 1.;
        if (maxrms != 0.) {
            hcoeff = 1. / maxrms;
        }

        m_path.moveTo(0, chan[0] + current_height + h / 2.);
        for (int i = 0; i < rmsv.size(); ++i) {
            m_path.lineTo(i, rmsv[i] * hcoeff * h / 2. + current_height + h / 2.);
        }
        m_path.lineTo(rmsv.size(), current_height + h / 2.);
        m_path.moveTo(0, chan[0] + current_height + h / 2.);
        for (int i = 0; i < rmsv.size(); ++i) {
            m_path.lineTo(i, -rmsv[i] * hcoeff * h / 2. + current_height + h / 2.);
        }
        m_path.lineTo(rmsv.size(), current_height + h / 2.);
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
