#include "nazar.h"

QRect centeredRect(const QPoint &pos, const QSize &size)
{
    const auto topLeft = pos - QPoint(size.width() / 2, size.height() / 2);
    return QRect(topLeft, size);
}

QPixmap screenshot(const QRect &rect)
{
    QPixmap result(rect.size());
    result.fill(Qt::black);

    const QList<QScreen *> screens = QApplication::screens();
    for (int i = 0; i < screens.size(); i++) {
        auto s = screens[i];
        if (s->geometry().intersects(rect)) {
            auto part = s->geometry().intersected(rect);
            auto winid = QApplication::desktop()->screen(i)->topLevelWidget()->winId();
            auto pix = s->grabWindow(winid, part.x(), part.y(), part.width(), part.height());
            QPainter painter(&result);
            painter.drawPixmap(part.topLeft() - rect.topLeft(), pix);
        }
    }

    return result;
}

void drawCrossHairs(QPainter *painter, const QRect &rect)
{
    PainterStateSaver saver(painter);

    painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
    painter->setBackground(Qt::white);
    painter->setBackgroundMode(Qt::OpaqueMode);

    painter->drawLine(rect.left(),  rect.center().y(),
                      rect.right(), rect.center().y());
    painter->drawLine(rect.center().x(), rect.top(),
                      rect.center().x(), rect.bottom());
}

QPixmap gradientPixmap(const QSize &s, const QGradient &g)
{
    QPixmap pix(s);
    pix.fill(Qt::transparent);

    QPainter p(&pix);
    p.fillRect(pix.rect(), g);

    return pix;
}

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QApplication a(argc, argv);

#ifdef Q_OS_WIN
    const int iconSize = 16;
#else
    const int iconSize = 22;
#endif

    QLinearGradient gradient(0, 0, iconSize, iconSize);
    gradient.setStops({
        { 0.0, Qt::red },
        { 0.5, Qt::green },
        { 1.0, Qt::blue }
    });

    const auto icon = gradientPixmap(QSize(iconSize, iconSize), gradient);
    a.setWindowIcon(icon);

    Viewer w;
    QSystemTrayIcon tray;
    QMenu trayMenu;

    tray.setIcon(icon);
    trayMenu.addAction("Exit", &a, &QApplication::quit);
    tray.setContextMenu(&trayMenu);

    w.show();
    tray.show();

    QObject::connect(&tray, &QSystemTrayIcon::activated, [&] (QSystemTrayIcon::ActivationReason reason) {
        switch (reason) {
        case QSystemTrayIcon::Context:
            break;
        case QSystemTrayIcon::Unknown:
        case QSystemTrayIcon::DoubleClick:
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::MiddleClick:
            w.show();
            break;
        }
    });

    return a.exec();
}
