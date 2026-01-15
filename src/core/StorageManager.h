#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include <QString>
#include <QJsonObject>
#include <QDir>

class StorageManager{
public: 
    static StorageManager& instance();
    
    void init();
    
    void saveConfig(const QString &key, const QVariant &value);
    QVariant loadConfig(const QString &key, const QVariant &defaultValue = QVariant());
private:
    StorageManager();

    QString rootPath; 
    QString configPath;

    void saveJson(const QJsonObject &json, const QString &path);
    QJsonObject loadJson(const QString&path);
};

#endif