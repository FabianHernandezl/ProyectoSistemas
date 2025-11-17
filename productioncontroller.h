#ifndef PRODUCTIONCONTROLLER_H
#define PRODUCTIONCONTROLLER_H

#include <QObject>
#include <QList>
#include <QTimer>
#include <QWidget>

#include "buffer.h"
#include "station.h"
#include "product.h"

// Hilos de mantenimiento
#include "generalcleanthread.h"
#include "generallogthread.h"
#include "generalstatsthread.h"

class ProductionController : public QObject
{
    Q_OBJECT

public:
    explicit ProductionController(QObject *parent = nullptr);
    ~ProductionController();

    // Control general de la producción
    void startProduction();
    void pauseProduction();
    void resetProductCounter();

    // Control de estaciones
    void stopAllStations();
    void startSpecificStationDialog(QWidget *parent);
    void pauseSpecificStationDialog(QWidget *parent);
    void stopSpecificStationDialog(QWidget *parent);

    // Control de hilos de mantenimiento (ESTO ES LO QUE TE FALTABA)
    void startMaintenanceThreads();
    void pauseMaintenanceThreads();

signals:
    void logMessage(const QString &msg);
    void updateProcessedCount(int value);
    void updateActiveWorkers(int value);

    void processEvent(const QString &station,
                      int productId,
                      const QString &state,
                      const QString &timestamp);

public slots:
    void handleStationEvent(const QString &station,
                            int productId,
                            const QString &state,
                            const QString &timestamp);

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

    // HILOS DE MANTENIMIENTO
    GeneralCleanThread *cleanThread_ = nullptr;
    GeneralLogThread   *logThread_ = nullptr;
    GeneralStatsThread *statsThread_ = nullptr;
};

#endif // PRODUCTIONCONTROLLER_H
