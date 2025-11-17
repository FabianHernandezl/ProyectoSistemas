#ifndef GENERALCLEANTHREAD_H
#define GENERALCLEANTHREAD_H

#include <QThread>
#include <atomic>

class GeneralCleanThread : public QThread
{
    Q_OBJECT
public:
    explicit GeneralCleanThread(QObject *parent = nullptr)
        : QThread(parent), running_(true) {}

    void stop() { running_ = false; }

protected:
    void run() override
    {
        while (running_)
        {
            QThread::msleep(800);
            // Limpieza simulada
        }
    }

private:
    std::atomic<bool> running_;
};

#endif // GENERALCLEANTHREAD_H
