#ifndef OWNERSHIPGUARD_H
#define OWNERSHIPGUARD_H

#include <QHash>
#include <QMutex>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

class OwnershipGuard {
public:
    // Pide propiedad del producto
    static bool claim(int productId, const QString &station);

    // Libera propiedad del producto
    static void release(int productId);

    // 🔥 Reset total (cuando se elimina toda la producción)
    static void reset();

    // 🔥 Eliminar solo un producto
    static void removeProduct(int productId);

    // Persistencia opcional
    static QJsonObject toJson();
    static void fromJson(const QJsonObject &obj);

private:
    static QHash<int, QString> owner;  // productId → estación dueña
    static QMutex m;
};

#endif
