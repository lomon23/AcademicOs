#include "MetricManager.h"

MetricManager& MetricManager::instance() {
    static MetricManager instance;
    return instance;
}

MetricManager::MetricManager(QObject *parent) : QObject(parent) {
    load(); // Завантажуємо дані при старті
    
    // Якщо це перший запуск і метрик немає - створимо тестові (для зручності розробки)
    if (definitions.isEmpty()) {
        registerMetric("mood", "Mood", MetricType::Slider, "/10", 1, 10);
        registerMetric("daily_spent", "Money Spent", MetricType::Money, "UAH", 0, 100000);
        registerMetric("sleep", "Sleep Hours", MetricType::Number, "h", 0, 24);
        save();
    }
}

MetricManager::~MetricManager() {
    save();
}

// --- LOGIC ---

void MetricManager::registerMetric(const QString &id, const QString &name, MetricType type, const QString &unit, double min, double max) {
    MetricDefinition def;
    def.id = id;
    def.name = name;
    def.type = type;
    def.unit = unit;
    def.min = min;
    def.max = max;

    definitions[id] = def;
    save(); // Зберігаємо конфігурацію відразу
}

QList<MetricDefinition> MetricManager::getAllDefinitions() const {
    return definitions.values();
}

bool MetricManager::hasMetric(const QString &id) const {
    return definitions.contains(id);
}

void MetricManager::logValue(const QString &id, const QDate &date, const QVariant &value) {
    if (!definitions.contains(id)) return; // Не можна писати в неіснуючу метрику

    QString dateKey = date.toString(Qt::ISODate);
    dataStore[id][dateKey] = value;
    
    qDebug() << "Logged:" << id << value << "for" << dateKey;
    save();
}

QVariant MetricManager::getValue(const QString &id, const QDate &date) const {
    QString dateKey = date.toString(Qt::ISODate);
    if (dataStore.contains(id) && dataStore[id].contains(dateKey)) {
        return dataStore[id][dateKey];
    }
    return QVariant();
}

QMap<QDate, double> MetricManager::getHistory(const QString &id, const QDate &from, const QDate &to) const {
    QMap<QDate, double> result;
    
    if (!dataStore.contains(id)) return result;

    const auto &records = dataStore[id];
    
    // Проходимо по всіх днях в діапазоні
    for (QDate d = from; d <= to; d = d.addDays(1)) {
        QString key = d.toString(Qt::ISODate);
        if (records.contains(key)) {
            result[d] = records[key].toDouble();
        } else {
            // Можна заповнювати нулями або пропускати (залежить від логіки графіка)
            result[d] = 0.0; 
        }
    }
    return result;
}

// --- FILE IO ---

QString MetricManager::getFilePath() const {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(path);
    if (!dir.exists()) dir.mkpath(".");
    return path + "/metrics.json";
}

void MetricManager::save() {
    QJsonObject root;

    // 1. Зберігаємо Визначення (Definitions)
    QJsonArray defArray;
    for (const auto &def : definitions) {
        defArray.append(def.toJson());
    }
    root["definitions"] = defArray;

    // 2. Зберігаємо Дані (Data)
    QJsonObject dataObj;
    for (auto it = dataStore.begin(); it != dataStore.end(); ++it) {
        QString metricId = it.key();
        QJsonObject dayEntries;
        
        QMap<QString, QVariant> days = it.value();
        for (auto dayIt = days.begin(); dayIt != days.end(); ++dayIt) {
            // QVariant автоматично конвертується в правильний тип JSON
            dayEntries[dayIt.key()] = QJsonValue::fromVariant(dayIt.value());
        }
        dataObj[metricId] = dayEntries;
    }
    root["data"] = dataObj;

    // Запис у файл
    QFile file(getFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
        file.close();
    }
}

void MetricManager::load() {
    QFile file(getFilePath());
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject root = doc.object();

    // 1. Завантажуємо Визначення
    QJsonArray defArray = root["definitions"].toArray();
    definitions.clear();
    for (const auto &val : defArray) {
        MetricDefinition def = MetricDefinition::fromJson(val.toObject());
        definitions[def.id] = def;
    }

    // 2. Завантажуємо Дані
    QJsonObject dataObj = root["data"].toObject();
    dataStore.clear();
    for (auto it = dataObj.begin(); it != dataObj.end(); ++it) {
        QString metricId = it.key();
        QJsonObject dayEntries = it.value().toObject();
        
        for (auto dayIt = dayEntries.begin(); dayIt != dayEntries.end(); ++dayIt) {
            dataStore[metricId][dayIt.key()] = dayIt.value().toVariant();
        }
    }
}