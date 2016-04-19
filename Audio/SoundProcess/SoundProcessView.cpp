#include "SoundProcessView.hpp"
#include <QPainter>

namespace Audio
{
namespace Sound
{

long LayerView::compareDensity(const double density) {

    if (m_density == -1 || m_density >= 4 * density || 4 * m_density <= density || (int)width() == 0 || m_curdata.empty()) {
        return RECOMPUTE_ALL;
    }
    if (m_density >= 2 * density) {
        return USE_NEXT;
    }
    if (2 * m_density <= density) {
        return USE_PREV;
    }
    return KEEP_CUR;
}

void LayerView::printAction(long action) {
    switch (action) {
    case KEEP_CUR:
        qDebug() << "keep current";
        break;
    case USE_NEXT :
        qDebug() << "use next";
        break;
    case USE_PREV :
        qDebug() << "use prev";
        break;
    case RECOMPUTE_ALL :
        qDebug() << "recompute all";
        break;
    default :
        qDebug() << "unknown action";
    }
}

std::vector<std::vector<double> > LayerView::computeDataSet(ZoomRatio ratio, double* densityptr)
{
    const int nchannels = m_data.size();

    const double density = std::max((m_sampleRate * ratio) / 1000., 1.);
    const int density_i = (int) density;
    const auto w = width();

    if (densityptr != nullptr)
        *densityptr = density;

    std::vector<std::vector<double> > dataset;
    dataset.resize(nchannels);
    for (int c = 0; c < nchannels; ++c) {

        const auto& chan = m_data[c];
        const auto chan_n = chan.size();

        const double length = (1000 * chan_n) / m_sampleRate; // duration of the track
        const double size = ratio > 0 ? length / ratio : 0; // number of pixels the track will occupy in its entirety

        const int npoints = std::max((int)w, (int)size); // number of points to draw on the screen

        std::vector<double>& rmsv = dataset[c];
        rmsv.reserve(npoints);

        for (int i = 0; i < npoints; ++i)
        {
            double rms = 0;
            for (int j = 0;
                 (j < density_i) && ((i * density_i + j) < chan.size());
                 ++j)
            {
                auto s = chan[i * density_i + j];
                rms += s * s;
            }
            rmsv.push_back(std::sqrt(rms / density));
        }
    }
    return dataset;
}

LayerView::LayerView(QGraphicsItem* parent):
    Process::LayerView{parent}
{
}

void LayerView::setData(const MediaFileHandle& data)
{
    m_data = data.data();
    m_sampleRate = data.sampleRate();
    m_density = -1;
}

void LayerView::drawWaveForms(ZoomRatio ratio) {

    const double density = std::max((ratio * m_sampleRate) / 1000., 1.);
    const double densityratio = (m_density > 0 && density > 0) ? m_density / density : 1.;

    int nchannels = m_data.size();
    if (nchannels == 0)
        return;

    // Height of each channel
    const auto h = height() / (double)nchannels;

    const int w = width();

    // Trace lines between channels

    for (int c = 1 ; c < nchannels; ++c) {
        m_channels.moveTo(0, c * h);
        m_channels.lineTo(w, c * h);
    }

    for (int c = 0; c < nchannels ; ++c) {
        const int current_height = c * h;
        std::vector<double> dataset = m_curdata[c];

        QPainterPath path{};
        path.setFillRule(Qt::WindingFill);

        // Draw path for current channel

        auto n = dataset.size();
        auto height_adjustemnt = current_height + h / 2.;
        if (dataset.size() > 0) {
            path.moveTo(0, dataset[0] + height_adjustemnt);
            for (int i = 0; i < n; ++i) {
                path.lineTo(i * densityratio, dataset[i] * h / 2. + height_adjustemnt);
            }
            path.lineTo(n * densityratio, height_adjustemnt);
            path.moveTo(0, dataset[0] + height_adjustemnt);
            for (int i = 0; i < n; ++i) {
                path.lineTo(i * densityratio, -dataset[i] * h / 2. + height_adjustemnt);
            }
            path.lineTo(n * densityratio, height_adjustemnt);
        }
        m_paths.push_back(path);
    }
}

void LayerView::recompute(const TimeValue& dur, ZoomRatio ratio)
{
    m_paths = QList<QPainterPath> ();
    m_channels = QPainterPath{};

    if(m_data.size() == 0)
        return;

    const int density = std::max((int)(ratio * m_sampleRate / 1000), 1);
    long action = compareDensity(density);

    switch (action) {
    case KEEP_CUR:
        break;
    case USE_NEXT:
        m_prevdata = std::vector<std::vector<double> > (m_curdata);
        m_prevdensity = m_density;
        m_curdata = std::vector<std::vector<double> > (m_nextdata);
        m_density = m_nextdensity;
        if (density > 1)
            m_nextdata = computeDataSet(ratio / 2, &m_nextdensity);
        break;
    case USE_PREV:
        m_nextdata = std::vector<std::vector<double> > (m_curdata);
        m_nextdensity = m_density;
        m_curdata = std::vector<std::vector<double> > (m_prevdata);
        m_density = m_prevdensity;
        m_prevdata = computeDataSet(2 * ratio, &m_prevdensity);
        break;
    case RECOMPUTE_ALL:
        m_prevdata = computeDataSet(2 * ratio, &m_prevdensity);
        m_curdata = computeDataSet(ratio, &m_density);
        m_nextdata = computeDataSet(ratio / 2, &m_nextdensity);
        break;
    default:
        break;
    }

    drawWaveForms(ratio);

    update();
}

void LayerView::paint_impl(QPainter* painter) const
{
    painter->setBrush(Qt::darkCyan);
    painter->setPen(Qt::darkBlue);

    for (auto path : m_paths)
        painter->drawPath(path);

    painter->setPen(Qt::lightGray);

    painter->drawPath(m_channels);
}

void LayerView::mousePressEvent(QGraphicsSceneMouseEvent*)
{
    emit pressed();
}
}
}
