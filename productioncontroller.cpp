#include "productioncontroller.h"
#include <QDebug>
#include <QDateTime>
#include <QMetaObject>
#include <QInputDialog>
#include <QMessageBox>

// =======================
//  LOGGER GLOBAL
// =======================
static ProductionController *g_controller = nullptr;

static void qtMessageHandler(QtMsgType,
                             const QMessageLogContext &,
                             const QString &msg)
{
    if (g_controller)
    {
        QMetaObject::invokeMethod(
            g_controller,
            "forwardDebugMessage",
            Qt::QueuedConnection,
            Q_ARG(QString, msg)
            );
    }
}

// =======================
//    CONSTRUCTOR
// =======================
ProductionController::ProductionController(QObject *parent)
    : QObject(parent)
{
    g_controller = this;
    qInstallMessageHandler(qtMessageHandler);
}

ProductionController::~ProductionController()
{
    pauseProduction();
    clearAll();
}

QString ProductionController::ts() const
{
    return QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
}

// =======================
//     LIMPIEZA
// =======================
void ProductionController::clearAll()
{
    qDeleteAll(stations_);
    stations_.clear();

    qDeleteAll(buffers_);
    buffers_.clear();
}

// =======================
//    INICIAR PRODUCCIÓN
// =======================
void ProductionController::startProduction()
{
    if (running_) return;

    running_ = true;
    emit logMessage(ts() + " ▶️ Starting production...");

    if (buffers_.isEmpty())
    {
        for (int i = 0; i < 5; ++i)
            buffers_.append(new Buffer(i, 3));

        stations_.append(new Station("Assembly",   buffers_[0], buffers_[1], 0, this));
        stations_.append(new Station("Painting",   buffers_[1], buffers_[2], 1, this));
        stations_.append(new Station("Inspection", buffers_[2], buffers_[3], 2, this));
        stations_.append(new Station("Packing",    buffers_[3], buffers_[4], 3, this));

        Station *shipping = new Station("Shipping", buffers_[4], nullptr, 4, this);
        shipping->setAsLastStation();
        stations_.append(shipping);

        for (auto *s : stations_)
        {
            connect(s, &Station::processEvent,
                    this, &ProductionController::handleStationEvent);
            s->start();
        }
    }

    if (!productTimer_)
    {
        productTimer_ = new QTimer(this);

        connect(productTimer_, &QTimer::timeout, this, [this]() {
            if (!running_) return;

            if (buffers_[0]->size() < buffers_[0]->capacity())
            {
                Product p(nextProductId_++, "Standard");
                buffers_[0]->insert(p);

                emit logMessage(ts() +
                                QString(" 📦 Inserted product %1 into buffer 0").arg(p.getId()));
            }
        });
    }

    productTimer_->start(800);
}

// =======================
//    PAUSAR PRODUCCIÓN
// =======================
void ProductionController::pauseProduction()
{
    if (!running_) return;

    running_ = false;
    emit logMessage(ts() + " ⏸️ Pausing production...");

    if (productTimer_)
        productTimer_->stop();
}

// =======================
//   MANEJO DE EVENTOS
// =======================
void ProductionController::forwardDebugMessage(const QString &msg)
{
    emit logMessage(msg);
}

void ProductionController::handleStationEvent(
    const QString &station,
    int productId,
    const QString &state,
    const QString &timestamp)
{
    emit processEvent(station, productId, state, timestamp);
}

// =======================
//   REINICIAR ID
// =======================
void ProductionController::resetProductCounter()
{
    nextProductId_ = 1;
}

// =======================
//   DETENER TODAS
// =======================
void ProductionController::stopAllStations()
{
    for (Station *s : stations_)
        s->stopStation();

    emit logMessage(ts() + " ⛔ Todas las estaciones detenidas.");
}

// =======================
//  DIALOGOS DE ESTACIONES
// =======================
void ProductionController::startSpecificStationDialog(QWidget *parent)
{
    QStringList names;
    for (auto *s : stations_)
        names << s->name();

    bool ok;
    QString selected = QInputDialog::getItem(
        parent, "Iniciar estación", "Seleccione la estación:",
        names, 0, false, &ok);

    if (!ok) return;

    int index = names.indexOf(selected);
    if (index < 0) return;

    stations_[index]->resumeStation();
}

void ProductionController::pauseSpecificStationDialog(QWidget *parent)
{
    QStringList names;
    for (auto *s : stations_)
        names << s->name();

    bool ok;
    QString selected = QInputDialog::getItem(
        parent, "Pausar estación", "Seleccione la estación:",
        names, 0, false, &ok);

    if (!ok) return;

    int index = names.indexOf(selected);
    if (index < 0) return;

    stations_[index]->pauseStation();
}

void ProductionController::stopSpecificStationDialog(QWidget *parent)
{
    QStringList names;
    for (auto *s : stations_)
        names << s->name();

    bool ok;
    QString selected = QInputDialog::getItem(
        parent, "Detener estación", "Seleccione la estación:",
        names, 0, false, &ok);

    if (!ok) return;

    int index = names.indexOf(selected);
    if (index < 0) return;

    stations_[index]->stopStation();
}

// =======================
//   HILOS DE MANTENIMIENTO
// =======================
void ProductionController::startMaintenanceThreads()
{
    if (!cleanThread_)
    {
        cleanThread_ = new GeneralCleanThread(this);
        cleanThread_->start();
    }

    if (!logThread_)
    {
        logThread_ = new GeneralLogThread(this);
        logThread_->start();
    }

    if (!statsThread_)
    {
        statsThread_ = new GeneralStatsThread(this);
        statsThread_->start();
    }

    emit logMessage(ts() + " 🔧 Hilos de mantenimiento iniciados.");
}

void ProductionController::pauseMaintenanceThreads()
{
    if (cleanThread_) cleanThread_->stop();
    if (logThread_) logThread_->stop();
    if (statsThread_) statsThread_->stop();

    emit logMessage(ts() + " 🚫 Hilos de mantenimiento detenidos.");
}
