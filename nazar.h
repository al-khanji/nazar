#ifndef NAZAR_H
#define NAZAR_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "viewer.h"

class PainterStateSaver
{
public:
    PainterStateSaver(QPainter *painter)
        : m_painter(painter)
    {
        if (m_painter) {
            m_painter->save();
        }
    }

    ~PainterStateSaver()
    {
        if (m_painter) {
            m_painter->restore();
        }
    }

private:
    QPainter *m_painter = nullptr;
};

extern QRect   centeredRect(const QPoint &pos, const QSize &size);
extern QPixmap screenshot(const QRect &rect);
extern void    drawCrossHairs(QPainter *painter, const QRect &rect);

#endif // NAZAR_H
