#include "producttracker.h"
#include <QDebug>
#include <QDateTime>    // Para timestamp

QHash<int, int> ProductTracker::expectedStage;
QMutex ProductTracker::m;

static inline QString ts2() {
    return QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
}

bool ProductTracker::checkAndAdvance(int productId, int stationIndex, const QString& stationName) {
    QMutexLocker lock(&m);
    int& exp = expectedStage[productId];  // Si no existe, se inicializa automáticamente en 0

    if (stationIndex != exp) {
        qWarning() << ts2()
        << "❌ SECUENCIA INVALIDA: Producto" << productId
        << "en" << stationName
        << "(idx" << stationIndex << "), se esperaba idx" << exp;
        return false;
    }

    // Avanzar expectativa para la siguiente estación
    exp++;

    qDebug() << ts2()
             << "✅ Secuencia OK: Producto" << productId
             << "pasó correctamente por idx" << stationIndex
             << "(" << stationName << ")";
    return true;
}
