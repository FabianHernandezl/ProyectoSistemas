#ifndef PRODUCTTRACKER_H
#define PRODUCTTRACKER_H

#include <QHash>
#include <QMutex>
#include <QJsonObject>
#include <QJsonArray>

class ProductTracker {
public:
    // Verifica orden correcto de estaciones
    static bool checkAndAdvance(int productId, int stationIndex, const QString &stationName);

    // 🔥 Reset total (al borrar producción o reiniciar)
    static void reset();

    // 🔥 Eliminar solo un producto (cuando se borra un proceso)
    static void removeProduct(int productId);

    // (Opcional) Persistencia
    static QJsonObject toJson();
    static void fromJson(const QJsonObject &obj);

private:
    static QHash<int,int> expectedStage;   // producto → stage esperado
    static QMutex m;
};

#endif
