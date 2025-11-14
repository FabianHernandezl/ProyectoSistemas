#ifndef PRODUCTTRACKER_H
#define PRODUCTTRACKER_H

#include <QHash>
#include <QMutex>

class ProductTracker {
public:
    // stage esperado por producto (0..4). Si no existe, se crea en 0.
    static bool checkAndAdvance(int productId, int stationIndex, const QString& stationName);

private:
    static QHash<int,int> expectedStage; // producto -> stage esperado
    static QMutex m;
};

#endif // PRODUCTTRACKER_H
