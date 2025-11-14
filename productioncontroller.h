#ifndef PRODUCTIONCONTROLLER_H
#define PRODUCTIONCONTROLLER_H

#include <QObject>
#include <QList>
#include <QTimer>
#include "buffer.h"
#include "station.h"
#include "product.h"

class ProductionController : public QObject
{
    Q_OBJECT

public:
    explicit ProductionController(QObject *parent = nullptr);
    ~ProductionController();

    void startProduction();
    void stopProduction();

    // --- Estadísticas ---
    void incrementProcessed() {
        processedCount_++;
        emit updateProcessedCount(processedCount_);
    }

    void incrementWorkers() {
        activeWorkers_++;
        emit updateActiveWorkers(activeWorkers_);
    }

    void decrementWorkers() {
        if (activeWorkers_ > 0)
            activeWorkers_--;
        emit updateActiveWorkers(activeWorkers_);
    }

signals:
    void logMessage(const QString &msg);
    void updateProcessedCount(int value);
    void updateActiveWorkers(int value);

    // Señal que se enviará a MainWindow
    void processEvent(const QString &station,
                      int productId,
                      const QString &state,
                      const QString &timestamp);

public slots:
    // SLOT que recibe señales de las estaciones
    void handleStationEvent(const QString &station,
                            int productId,
                            const QString &state,
                            const QString &timestamp);

private slots:
    void forwardDebugMessage(const QString &msg);

private:
    QString ts() const;
    void clearAll();

    QList<Buffer*> buffers_;
    QList<Station*> stations_;

    bool running_ = false;

    QTimer *productTimer_ = nullptr;
    int nextProductId_ = 1;

    int processedCount_ = 0;
    int activeWorkers_ = 0;
};

#endif
