#include "producttracker.h"
#include <QDateTime>
#include <QDebug>

QHash<int, int> ProductTracker::expectedStage;
QMutex ProductTracker::m;

static inline QString tsPT()
{
    return QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
}

bool ProductTracker::checkAndAdvance(int productId, int stationIndex, const QString &stationName)
{
    QMutexLocker lock(&m);

    int &expected = expectedStage[productId]; // si no existe, es 0

    if (stationIndex != expected)
    {
        qWarning() << tsPT()
        << "❌ ORDEN INCORRECTO → Producto" << productId
        << "está en" << stationName
        << "pero se esperaba estación índice" << expected;
        return false;
    }

    expected++;

    qDebug() << tsPT()
             << "✅ ORDEN OK → P" << productId
             << "pasó por" << stationName
             << "(idx" << stationIndex << ")";
    return true;
}

void ProductTracker::reset()
{
    QMutexLocker lock(&m);
    expectedStage.clear();
    qDebug() << tsPT() << "🔄 ProductTracker: Reset completo";
}

void ProductTracker::removeProduct(int productId)
{
    QMutexLocker lock(&m);
    expectedStage.remove(productId);
    qDebug() << tsPT() << "🗑️ ProductTracker: Eliminado P" << productId;
}

// OPCIONAL → persistencia
QJsonObject ProductTracker::toJson()
{
    QMutexLocker lock(&m);

    QJsonObject root;
    QJsonArray arr;

    for (auto it = expectedStage.begin(); it != expectedStage.end(); ++it)
    {
        QJsonObject obj;
        obj["productId"] = it.key();
        obj["stage"] = it.value();
        arr.append(obj);
    }
    root["tracker"] = arr;
    return root;
}

void ProductTracker::fromJson(const QJsonObject &obj)
{
    QMutexLocker lock(&m);

    expectedStage.clear();
    QJsonArray arr = obj["tracker"].toArray();

    for (const auto &item : arr)
    {
        QJsonObject o = item.toObject();
        expectedStage[o["productId"].toInt()] = o["stage"].toInt();
    }

    qDebug() << tsPT() << "📥 ProductTracker: Estado restaurado";
}
