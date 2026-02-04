#include "src/core/StorageManager.h"
#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

StorageManager& StorageManager::instance()
{
    static StorageManager instance;
    return instance;
}

StorageManager::StorageManager()
{
    // Визначаємо шлях: /home/user/AcademicOS
    QString home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    rootPath = home + "/AcademicOS";
    configPath = rootPath + "/config.json";
}

void StorageManager::init()
{
    QDir dir;
    
    // Створюємо папку AcademicOS, якщо її немає
    if (!dir.exists(rootPath)) {
        dir.mkpath(rootPath);
        qDebug() << "Created root folder:" << rootPath;
    }

    // Створюємо підпапки notes та data
    dir.mkpath(rootPath + "/notes");
    dir.mkpath(rootPath + "/data");
}

void StorageManager::saveConfig(const QString &key, const QVariant &value)
{
    // 1. Читаємо те, що вже є
    QJsonObject json = loadJson(configPath);
    
    // 2. Змінюємо потрібне значення (QVariant сам розбереться int це чи String)
    json[key] = QJsonValue::fromVariant(value);
    
    // 3. Зберігаємо назад
    saveJson(json, configPath);
}

QVariant StorageManager::loadConfig(const QString &key, const QVariant &defaultValue)
{
    QJsonObject json = loadJson(configPath);
    
    if (json.contains(key)) {
        return json[key].toVariant();
    }
    return defaultValue;
}

void StorageManager::saveJson(const QJsonObject &json, const QString &path)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(json);
        file.write(doc.toJson());
        file.close();
    } else {
        qWarning() << "Could not save file:" << path;
    }
}
QJsonObject StorageManager::loadJson(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return QJsonObject(); // Повертаємо пустий об'єкт, якщо файлу немає
    }

    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.object();
}