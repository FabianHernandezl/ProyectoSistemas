#include "ownershipguard.h"
QHash<int, QString> OwnershipGuard::owner;
QMutex OwnershipGuard::m;

bool OwnershipGuard::claim(int productId, const QString& station) {
    QMutexLocker L(&m);
    if (owner.contains(productId)) {
        qWarning() << "🚨 DUPLICADO: P" << productId
                   << "ya estaba en" << owner[productId]
                   << "y ahora llegó a" << station;
        return false;
    }
    owner[productId] = station;
    return true;
}

void OwnershipGuard::release(int productId) {
    QMutexLocker L(&m);
    owner.remove(productId);
}
