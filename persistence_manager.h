#ifndef PERSISTENCE_MANAGER_H
#define PERSISTENCE_MANAGER_H

#include <QJsonArray>
#include <QString>

class PersistenceManager
{
public:
    static QString filePath();

    // Versión original
    static void saveTable(const QJsonArray &rows);
    static QJsonArray loadTable();

    // Compatibilidad con MainWindow viejo
    static void saveEvents(const QJsonArray &rows);
    static QJsonArray loadEvents();
};

#endif
