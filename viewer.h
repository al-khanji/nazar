#ifndef VIEWER_H
#define VIEWER_H

#include "nazar.h"

namespace Ui {
class Viewer;
}

class Viewer : public QWidget
{
    Q_OBJECT

public:
    explicit Viewer(QWidget *parent = 0);
    ~Viewer();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::Viewer *ui = nullptr;
    int m_updateTimerId = 0;
    QVector<QColor> m_color_values;
    QVector<QString> m_color_names;

    int closestColorIndex(const QColor &color) const;
};

#endif // VIEWER_H
