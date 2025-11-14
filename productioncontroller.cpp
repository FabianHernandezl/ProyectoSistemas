#include "productioncontroller.h"
#include <QDebug>
#include <QDateTime>
#include <QMetaObject>
#include <QTimer>

//
// ───────────────────────────────────────────────
//     HANDLER GLOBAL DE MENSAJES (LOG)
// ───────────────────────────────────────────────
//
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

//
// ───────────────────────────────────────────────
//         CONSTRUCTOR / DESTRUCTOR
// ───────────────────────────────────────────────
//
ProductionController::ProductionController(QObject *parent)
    : QObject(parent)
{
    g_controller = this;
    qInstallMessageHandler(qtMessageHandler);
}

ProductionController::~ProductionController()
{
    stopProduction();
    clearAll();
}

//
// ───────────────────────────────────────────────
//               TIMESTAMP
// ───────────────────────────────────────────────
//
QString ProductionController::ts() const
{
    return QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
}

//
// ───────────────────────────────────────────────
//          LIMPIEZA COMPLETA
// ───────────────────────────────────────────────
//
void ProductionController::clearAll()
{
    qDeleteAll(stations_);
    stations_.clear();

    qDeleteAll(buffers_);
    buffers_.clear();
}

//
// ───────────────────────────────────────────────
//           INICIAR PRODUCCIÓN
// ───────────────────────────────────────────────
//
void ProductionController::startProduction()
{
    if (running_) return;

    running_ = true;
    emit logMessage(ts() + " ▶️ Starting production...");

    //
    // Crear buffers + estaciones SOLO 1 VEZ
    //
    if (buffers_.isEmpty())
    {
        for (int i = 0; i < 5; ++i)
            buffers_.append(new Buffer(i, 3));

        stations_.append(new Station("Assembly",   buffers_[0], buffers_[1], 0, this));
        stations_.append(new Station("Painting",   buffers_[1], buffers_[2], 1, this));
        stations_.append(new Station("Inspection", buffers_[2], buffers_[3], 2, this));
        stations_.append(new Station("Packing",    buffers_[3], buffers_[4], 3, this));

        // SHIPPING — última estación
        stations_.append(new Station("Shipping", buffers_[4], nullptr, 4, this));
        stations_.last()->setAsLastStation();

        //
        // CONECTAR: cada estación emite processEvent → controlador
        //
        for (auto *s : stations_)
        {
            connect(s, &Station::processEvent,
                    this, &ProductionController::handleStationEvent);

            s->start();
        }
    }

    //
    // Timer para insertar productos
    //
    if (!productTimer_)
    {
        productTimer_ = new QTimer(this);

        connect(productTimer_, &QTimer::timeout, this, [this]() {

            if (!running_) return;

            if (buffers_[0]->size() < buffers_[0]->capacity())
            {
                Product p(nextProductId_++, "Standard");
                buffers_[0]->insert(p);

                emit logMessage(
                    ts() + QString(" 📦 Inserted product %1 into buffer 0").arg(p.getId()));
            }
        });
    }

    productTimer_->start(800);
}

//
// ───────────────────────────────────────────────
//           DETENER PRODUCCIÓN
// ───────────────────────────────────────────────
//
void ProductionController::stopProduction()
{
    if (!running_) return;

    running_ = false;

    emit logMessage(ts() + " ⏸️ Pausing production...");

    if (productTimer_)
        productTimer_->stop();
}

//
// ───────────────────────────────────────────────
//           LOG DE MENSAJES DE QT
// ───────────────────────────────────────────────
//
void ProductionController::forwardDebugMessage(const QString &msg)
{
    emit logMessage(msg);
}

//
// ───────────────────────────────────────────────
//  SLOT: EVENTO RECIBIDO DESDE UNA ESTACIÓN
// ───────────────────────────────────────────────
//
void ProductionController::handleStationEvent(
    const QString &station,
    int productId,
    const QString &state,
    const QString &timestamp)
{
    emit processEvent(station, productId, state, timestamp);
}
