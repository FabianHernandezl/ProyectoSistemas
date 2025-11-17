#include "persistence_manager.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

QString PersistenceManager::filePath()
{
    QString path = QDir::homePath() + "/ProyectoSistemasData";
    QDir().mkpath(path);
    return path + "/process_table.json";
}

void PersistenceManager::saveTable(const QJsonArray &rows)
{
    QJsonObject root;
    root["events"] = rows;

    QJsonDocument doc(root);

    QFile out(filePath());
    if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "❌ No se pudo abrir el archivo JSON para escribir.";
        return;
    }

    out.write(doc.toJson());
    out.close();
}

QJsonArray PersistenceManager::loadTable()
{
    QFile in(filePath());
    if (!in.open(QIODevice::ReadOnly))
        return QJsonArray();

    QJsonDocument doc = QJsonDocument::fromJson(in.readAll());
    in.close();

    if (!doc.isObject())
        return QJsonArray();

    QJsonObject root = doc.object();
    if (root.contains("events") && root["events"].isArray())
        return root["events"].toArray();

    return QJsonArray();
}

//
// --- COMPATIBILIDAD PARA MainWindow ---
//

void PersistenceManager::saveEvents(const QJsonArray &rows)
{
    saveTable(rows);
}

QJsonArray PersistenceManager::loadEvents()
{
    return loadTable();
}
