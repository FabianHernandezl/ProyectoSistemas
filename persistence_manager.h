#ifndef PERSISTENCEMANAGER_H
#define PERSISTENCEMANAGER_H

#include <QJsonArray>
#include <QString>

class PersistenceManager
{
public:
    static QString filePath();
    static void saveTable(const QJsonArray &rows);
    static QJsonArray loadTable();  // Método para cargar los datos desde el archivo JSON
};

#endif
