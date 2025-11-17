#ifndef GENERALSTATSTHREAD_H
#define GENERALSTATSTHREAD_H

#include <QThread>
#include <atomic>

class GeneralStatsThread : public QThread
{
    Q_OBJECT
public:
    explicit GeneralStatsThread(QObject *parent = nullptr)
        : QThread(parent), running_(true) {}

    void stop() { running_ = false; }

protected:
    void run() override
    {
        while (running_)
        {
            QThread::msleep(700);
            // Cálculo de estadísticas simulado
        }
    }

private:
    std::atomic<bool> running_;
};

#endif // GENERALSTATSTHREAD_H
