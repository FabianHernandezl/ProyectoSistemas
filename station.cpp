#include "station.h"
#include "productioncontroller.h"
#include <QTime>
#include <QRandomGenerator>
#include <QDebug>

QString Station::ts() const
{
    return QTime::currentTime().toString("hh:mm:ss.zzz");
}

Station::Station(const QString &name,
                 Buffer *input,
                 Buffer *output,
                 int index,
                 ProductionController *controller)
    : name_(name),
    input_(input),
    output_(output),
    index_(index),
    controller_(controller)
{
}

void Station::stop()
{
    requestInterruption();
}

void Station::run()
{
    while (!isInterruptionRequested())
    {
        // Esperar producto sin incrementar recurso todavía
        Product p = input_->remove();

        // Si el producto es la señal de parada
        if (p.isPoison())
        {
            emit processEvent(name_, -1, "Stop signal", ts());
            break;
        }

        // 🔸 AHORA SÍ: trabajador está activo
        if (controller_)
            controller_->incrementWorkers();

        emit processEvent(name_, p.id(), "Recibido", ts());

        p.advanceState(name_);
        emit processEvent(name_, p.id(), "Procesando", ts());

        QThread::msleep(600 + QRandomGenerator::global()->bounded(800));

        if (output_)
        {
            output_->insert(p);
            emit processEvent(name_, p.id(), "Enviado", ts());
        }
        else
        {
            emit processEvent(name_, p.id(), "Finalizado", ts());

            if (controller_ && isLastStation_)
                controller_->incrementProcessed();
        }

        // 🔸 Trabajo finalizado → restar recurso
        if (controller_)
            controller_->decrementWorkers();
    }

    emit processEvent(name_, -1, "Stopped", ts());
}
