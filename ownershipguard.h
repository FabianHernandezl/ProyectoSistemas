#ifndef OWNERSHIPGUARD_H
#define OWNERSHIPGUARD_H

#include <QHash>
#include <QMutex>
#include <QDebug>

class OwnershipGuard {
public:
    static bool claim(int productId, const QString& station);
    static void release(int productId);

private:
    static QHash<int, QString> owner;
    static QMutex m;
};

#endif // OWNERSHIPGUARD_H
