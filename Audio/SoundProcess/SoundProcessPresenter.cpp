#include "SoundProcessPresenter.hpp"
#include "SoundProcessView.hpp"
#include <iscore/document/DocumentContext.hpp>
namespace Audio
{
namespace Sound
{
LayerPresenter::LayerPresenter(
        const LayerModel& layer,
        LayerView* view,
        QObject* parent):
    Process::LayerPresenter{"AudioLayerPresenter", parent},
    m_layer{layer},
    m_view{view},
    m_focusDispatcher{iscore::IDocument::documentContext(layer).document}
{
    connect(view, &LayerView::pressed,
            this, [&] () {
        m_focusDispatcher.focus(this);
    });

    auto& pm = processModel();
    con(pm, &ProcessModel::fileChanged,
        this, [&] () {
        m_view->setData(model(*this).file());
        m_view->recompute(model(*this).duration(), m_ratio);
    });

    m_view->setData(model(*this).file());
    m_view->recompute(model(*this).duration(), m_ratio);
}

void LayerPresenter::setWidth(qreal val)
{
    m_view->setWidth(val);
    m_view->recompute(processModel().duration(), m_ratio);
}

void LayerPresenter::setHeight(qreal val)
{
    m_view->setHeight(val);
    m_view->recompute(processModel().duration(), m_ratio);
}

void LayerPresenter::putToFront()
{
    m_view->recompute(processModel().duration(), m_ratio);
    m_view->show();
}

void LayerPresenter::putBehind()
{
    m_view->hide();
}

void LayerPresenter::on_zoomRatioChanged(ZoomRatio r)
{
    m_ratio = r;
    m_view->recompute(processModel().duration(), m_ratio);
}

void LayerPresenter::parentGeometryChanged()
{
    m_view->recompute(processModel().duration(), m_ratio);
}

const LayerModel& LayerPresenter::layerModel() const
{
    return m_layer;
}

const Id<Process::ProcessModel>& LayerPresenter::modelId() const
{
    return processModel().id();
}

void LayerPresenter::fillContextMenu(
        QMenu*,
        const QPoint&,
        const QPointF&) const
{
}

}
}
