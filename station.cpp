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
    running_ = true;

    while (running_)
    {
        // -------- PAUSA --------
        while (paused_ && running_)
        {
            QThread::msleep(100);
        }

        if (!running_) break;

        // -------- LEER DEL BUFFER --------
        Product p = input_->remove();


        emit processEvent(name_, p.getId(), "Procesando", ts());

        // Simulación de trabajo
        QThread::msleep(800);

        if (isLastStation_)
        {
            emit processEvent(name_, p.getId(), "Finalizado", ts());
        }
        else
        {
            output_->insert(p);
            emit processEvent(name_, p.getId(), "Enviado", ts());
        }
    }

    emit processEvent(name_, -1, "Detenida", ts());
}

