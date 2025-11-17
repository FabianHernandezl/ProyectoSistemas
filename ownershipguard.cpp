#include "ownershipguard.h"
#include <QDateTime>

QHash<int, QString> OwnershipGuard::owner;
QMutex OwnershipGuard::m;

static inline QString tsOG()
{
    return QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
}

bool OwnershipGuard::claim(int productId, const QString &station)
{
    QMutexLocker lock(&m);

    if (owner.contains(productId))
    {
        qWarning() << tsOG()
        << "🚨 DUPLICADO → P" << productId
        << "ya estaba siendo procesado por"
        << owner[productId]
        << "y ahora llegó a" << station;
        return false;
    }

    owner[productId] = station;

    qDebug() << tsOG()
             << "🔐 Ownership OK → P" << productId
             << "asignado a" << station;

    return true;
}

void OwnershipGuard::release(int productId)
{
    QMutexLocker lock(&m);

    if (owner.remove(productId) > 0)
    {
        qDebug() << tsOG() << "🔓 Ownership liberado → P" << productId;
    }
}

void OwnershipGuard::reset()
{
    QMutexLocker lock(&m);
    owner.clear();
    qDebug() << tsOG() << "🔄 OwnershipGuard: Reset completo";
}

void OwnershipGuard::removeProduct(int productId)
{
    QMutexLocker lock(&m);
    owner.remove(productId);
    qDebug() << tsOG() << "🗑️ OwnershipGuard: Eliminado P" << productId;
}

// OPCIONAL → persistencia
QJsonObject OwnershipGuard::toJson()
{
    QMutexLocker lock(&m);

    QJsonObject root;
    QJsonArray arr;

    for (auto it = owner.begin(); it != owner.end(); ++it)
    {
        QJsonObject obj;
        obj["productId"] = it.key();
        obj["station"] = it.value();
        arr.append(obj);
    }
    root["ownership"] = arr;
    return root;
}

void OwnershipGuard::fromJson(const QJsonObject &obj)
{
    QMutexLocker lock(&m);

    owner.clear();
    QJsonArray arr = obj["ownership"].toArray();

    for (const auto &item : arr)
    {
        QJsonObject o = item.toObject();
        owner[o["productId"].toInt()] = o["station"].toString();
    }

    qDebug() << tsOG() << "📥 Ownership restaurado desde JSON";
}
