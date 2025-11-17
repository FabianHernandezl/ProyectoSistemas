#ifndef STATION_H
#define STATION_H

#include <QThread>
#include <atomic>
#include "buffer.h"
#include "product.h"

class ProductionController;

class Station : public QThread
{
    Q_OBJECT

public:
    Station(const QString &name,
            Buffer *input,
            Buffer *output,
            int index,
            ProductionController *controller);

    // Getter del nombre (solo uno)
    QString name() const { return name_; }

    // === LO QUE EL PROFESOR PIDIÓ ===
    void pauseStation();
    void resumeStation();
    void stopStation();

    void setAsLastStation() { isLastStation_ = true; }

signals:
    void processEvent(const QString &station,
                      int id,
                      const QString &state,
                      const QString &time);

protected:
    void run() override;

private:
    QString ts() const;

    QString name_;
    Buffer *input_;
    Buffer *output_;
    int index_;

    ProductionController *controller_;

    bool isLastStation_ = false;

    // CONTROL DEL HILO
    std::atomic<bool> running_{true};
    std::atomic<bool> paused_{false};
};

#endif
