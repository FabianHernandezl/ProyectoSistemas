#include "persistence_manager.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

QString PersistenceManager::filePath()
{
    QString path = QDir::homePath() + "/ProyectoSistemasData";

    // Crear la carpeta si no existe
    QDir().mkpath(path);

    // Crear el archivo dentro de la carpeta
    return path + "/process_table.json";
}

void PersistenceManager::saveTable(const QJsonArray &rows)
{
    QJsonObject root;
    root["events"] = rows;

    QJsonDocument doc(root);

    QString file = filePath();
    QFile out(file);

    if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qWarning() << "❌ No se pudo abrir el archivo JSON para escribir:" << file;
        return;
    }

    out.write(doc.toJson());
    out.close();

    //qDebug() << "💾 JSON guardado en:" << file;
}

QJsonArray PersistenceManager::loadTable()
{
    QString file = filePath();  // Obtener la ruta del archivo

    QFile in(file);

    if (!in.open(QIODevice::ReadOnly))
    {
        qWarning() << "❌ No se pudo abrir el archivo JSON para leer:" << file;
        return QJsonArray();  // Si no se puede abrir el archivo, devolver un arreglo vacío
    }

    // Leer el archivo JSON
    QByteArray data = in.readAll();
    in.close();

    // Convertir el JSON a un documento
    QJsonDocument doc = QJsonDocument::fromJson(data);

    // Verificar si es un objeto JSON y extraer el array de "events"
    if (doc.isObject()) {
        QJsonObject root = doc.object();
        if (root.contains("events") && root["events"].isArray()) {
            return root["events"].toArray();  // Retornar el QJsonArray con los eventos
        }
    }

    return QJsonArray();  // Si no es un formato esperado, devolver un arreglo vacío
}

