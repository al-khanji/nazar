#include "nazar.h"
#include "ui_viewer.h"

static const int UpdateInterval = 200;
static const int CrossHairMargins = 20;

Viewer::Viewer(QWidget *parent)
    : QWidget(parent, Qt::WindowStaysOnTopHint)
    , ui(new Ui::Viewer)
{
    ui->setupUi(this);

    setWindowTitle(QCoreApplication::applicationName());

    QFile f(":/dataset.json");
    if (f.open(QIODevice::ReadOnly)) {
        QJsonParseError err;
        auto doc = QJsonDocument::fromJson(f.readAll(), &err);

        if (doc.isNull()) {
            qDebug() << "could not read colordb:" << err.errorString();
        } else if (doc.isArray()) {
            const auto arr = doc.array();

            m_color_values.reserve(arr.size());
            m_color_names.reserve(arr.size());

            QColor color;
            QString name;

            for (const auto &val : arr) {
                if (val.isObject()) {
                    const auto obj = val.toObject();
                    color = QColor(obj.value("x").toInt(),
                                   obj.value("y").toInt(),
                                   obj.value("z").toInt());
                    name = obj.value("label").toString();

                    if (color.isValid() && !name.isEmpty()) {
                        m_color_values.append(color);
                        m_color_names.append(name);
                    }
                }
            }
        }
    }
}

Viewer::~Viewer()
{
    delete ui;
}

void Viewer::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    m_updateTimerId = startTimer(UpdateInterval);
}

void Viewer::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);

    if (m_updateTimerId > 0)
        killTimer(m_updateTimerId);

    m_updateTimerId = 0;
}

void Viewer::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_updateTimerId) {
        const qreal dpr = this->topLevelWidget()->windowHandle()->screen()->devicePixelRatio();

        auto pix = screenshot(centeredRect(QCursor::pos(), ui->label_screenshot->size() * dpr));
        pix.setDevicePixelRatio(dpr);

        pix = pix.scaled(pix.size() * ui->spinBox_zoomLevel->value() * dpr, Qt::KeepAspectRatio, Qt::FastTransformation);
        pix = pix.copy(centeredRect(pix.rect().center(), ui->label_screenshot->size() * dpr));

        const auto color = QColor::fromRgb(pix.toImage().pixel(pix.rect().center()));
        ui->label_pixel->setText(color.name());

        const int match = closestColorIndex(color);

        if (match < 0) {
            ui->label_colorinfo->clear();
        } else {
            ui->label_colorinfo->setText(tr("%2 %1").arg(m_color_names[match]).arg(m_color_values[match].name()));
        }

        {
            QPainter painter(&pix);
            painter.setWorldMatrix(QMatrix().scale(1.0 / dpr, 1.0 / dpr));
            drawCrossHairs(&painter, pix.rect().adjusted( CrossHairMargins,  CrossHairMargins,
                                                         -CrossHairMargins, -CrossHairMargins));
        }

        ui->label_screenshot->setPixmap(pix);
    }

    QWidget::timerEvent(event);
}

void Viewer::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}

static double colorDistance(const QColor &c1, const QColor &c2)
{
    const double x = c1.redF() - c2.redF();
    const double y = c1.greenF() - c2.greenF();
    const double z = c1.blueF() - c2.blueF();
    return sqrt((x * x) + (y * y) + (z * z));
}

int Viewer::closestColorIndex(const QColor &color) const
{
    double distance = INFINITY;
    int index = -1;

    for (int i = 0; i < m_color_values.size(); ++i) {
        auto dd = colorDistance(color, m_color_values[i]);
        if (dd < distance) {
            distance = dd;
            index = i;
        }
    }

    return index;
}
