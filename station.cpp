#include "station.h"
#include <QTime>
#include <QRandomGenerator>
#include <QDebug>
#include <QThread>

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

// =======================
// PAUSAR
// =======================
void Station::pauseStation()
{
    paused_ = true;
}

// =======================
// REANUDAR
// =======================
void Station::resumeStation()
{
    paused_ = false;
}

// =======================
// DETENER COMPLETAMENTE
// =======================
void Station::stopStation()
{
    running_ = false;
}

void Station::run()
{
    while (running_)
    {
        // Pausa activa
        while (paused_ && running_)
        {
            QThread::msleep(100);
        }

        if (!running_) break;

        // Tomar producto
        Product p = input_->remove();

        if (p.isPoison())
        {
            emit processEvent(name_, -1, "Stop signal", ts());
            break;
        }

        emit processEvent(name_, p.id(), "Recibido", ts());

        p.advanceState(name_);
        emit processEvent(name_, p.id(), "Procesando", ts());

        QThread::msleep(600 + QRandomGenerator::global()->bounded(800));

        if (output_)
            output_->insert(p);
        else
            emit processEvent(name_, p.id(), "Finalizado", ts());
    }

    emit processEvent(name_, -1, "Stopped", ts());
}
