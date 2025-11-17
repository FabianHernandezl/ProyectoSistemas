#ifndef GENERALLOGTHREAD_H
#define GENERALLOGTHREAD_H

#include <QThread>
#include <atomic>

class GeneralLogThread : public QThread
{
    Q_OBJECT

public:
    explicit GeneralLogThread(QObject *parent = nullptr)
        : QThread(parent), running_(true) {}

    void stop() { running_ = false; }

protected:
    void run() override
    {
        while (running_)
        {
            QThread::msleep(500);
            // Aquí podrías emitir logs o simular actividad
        }
    }

private:
    std::atomic<bool> running_;
};

#endif // GENERALLOGTHREAD_H
