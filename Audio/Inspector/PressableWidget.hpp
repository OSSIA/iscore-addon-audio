#pragma once
#include <QTableWidget>

class PressableWidget : public QTableWidget
{
        Q_OBJECT
    public:
        using QTableWidget::QTableWidget;

signals:
        void pressed();

    private:
        void mousePressEvent(QMouseEvent* e) override
        {
            emit pressed();
            return QTableWidget::mousePressEvent(e);
        }
};
