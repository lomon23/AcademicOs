#include "src/modules/analytics/core/AnalyticsService.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>
#include <QUuid>
#include <QDebug>
#include <QRandomGenerator>

// =========================================================
// Singleton & Setup
// =========================================================

AnalyticsService& AnalyticsService::instance() {
    static AnalyticsService instance;
    return instance;
}

AnalyticsService::AnalyticsService() {
    loadData();
    
    // Якщо файл пустий або це перший запуск - створюємо дефолтну вкладку
    if (categoriesList.empty()) {
        addCategory("General");
    }
}

QString AnalyticsService::getFilePath() const {
    QString path = QDir::homePath() + "/AcademicOS";
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return path + "/analytics_data.json";
}

// =========================================================
// Save / Load (JSON Magic)
// =========================================================

void AnalyticsService::saveData() {
    QJsonObject root;

    // 1. ЗБЕРІГАЄМО СПИСОК КАТЕГОРІЙ (Табів)
    QJsonArray catsArray;
    for (const QString &cat : categoriesList) {
        catsArray.append(cat);
    }
    root["categories"] = catsArray;

    // 2. ЗБЕРІГАЄМО МЕТРИКИ
    QJsonArray metricsArray;
    for (const auto &metric : metricsList) {
        QJsonObject metricObj;
        metricObj["id"] = metric.id;
        metricObj["name"] = metric.name;
        metricObj["category"] = metric.category; 
        metricObj["isVisible"] = metric.isVisible;
        metricObj["color"] = metric.color;
        metricObj["units"] = metric.units;
        metricObj["minVal"] = metric.minVal;
        metricObj["maxVal"] = metric.maxVal;

        QJsonObject historyObj;
        QMapIterator<QString, double> i(metric.history);
        while (i.hasNext()) {
            i.next();
            historyObj[i.key()] = i.value();
        }
        metricObj["values"] = historyObj;
        metricsArray.append(metricObj);
    }
    root["metrics"] = metricsArray;

    // 3. Записуємо у файл
    QFile file(getFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
        file.close();
        qDebug() << "📊 Analytics saved categories & metrics to:" << getFilePath();
    } else {
        qWarning() << "⚠️ Could not save analytics!";
    }
}

void AnalyticsService::loadData() {
    QFile file(getFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "ℹ️ No analytics file found, starting fresh.";
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) return;

    QJsonObject root = doc.object();

    // 1. ЧИТАЄМО КАТЕГОРІЇ
    categoriesList.clear();
    if (root.contains("categories")) {
        QJsonArray catsArray = root["categories"].toArray();
        for (const auto &val : catsArray) {
            categoriesList.append(val.toString());
        }
    }
    
    // Захист: якщо файл старий і категорій нема -> додаємо General
    if (categoriesList.empty()) {
        categoriesList.append("General");
    }

    // 2. ЧИТАЄМО МЕТРИКИ
    metricsList.clear();
    QJsonArray metricsArray = root["metrics"].toArray();

    for (const auto &val : metricsArray) {
        QJsonObject obj = val.toObject();
        Metric m;
        m.id = obj["id"].toString();
        m.name = obj["name"].toString();
        
        // Якщо категорії нема (старий файл) - пишемо "General"
        m.category = obj.contains("category") ? obj["category"].toString() : "General";
        m.isVisible = obj.contains("isVisible") ? obj["isVisible"].toBool() : true;

        m.color = obj.contains("color") ? obj["color"].toString() : "#BD93F9"; // Фіолетовий за замовчуванням
        m.units = obj.contains("units") ? obj["units"].toString() : "";
        m.minVal = obj.contains("minVal") ? obj["minVal"].toDouble() : 0.0;
        m.maxVal = obj.contains("maxVal") ? obj["maxVal"].toDouble() : 0.0;
        QJsonObject historyObj = obj["values"].toObject();
        for (auto it = historyObj.begin(); it != historyObj.end(); ++it) {
            m.history.insert(it.key(), it.value().toDouble());
        }

        metricsList.push_back(m);
    }
    qDebug() << "📊 Loaded" << metricsList.size() << "metrics and" << categoriesList.size() << "categories.";
}

// =========================================================
// Public Logic
// =========================================================
Metric AnalyticsService::createMetric(const QString &name, const QString &category, 
                                      const QString &color, const QString &units,
                                      double minVal, double maxVal) {
    Metric m;
    m.id = QUuid::createUuid().toString();
    m.name = name;
    m.category = category;
    m.color = color;
    m.units = units;

    // Нові поля
    m.minVal = minVal;
    m.maxVal = maxVal;

    m.isVisible = true;
    metricsList.push_back(m);
    saveData();
    return m;
}

void AnalyticsService::deleteMetric(const QString &id) {
    metricsList.erase(std::remove_if(metricsList.begin(), metricsList.end(),
                                     [&id](const Metric& m) { return m.id == id; }),
                      metricsList.end());
    saveData();
}

std::vector<Metric> AnalyticsService::getAllMetrics() const {
    return metricsList;
}

std::vector<Metric> AnalyticsService::getMetricsByCategory(const QString &category) const {
    std::vector<Metric> filtered;
    for (const auto &m : metricsList) {
        // Шукаємо точний збіг категорії
        if (m.category == category) {
            filtered.push_back(m);
        }
    }
    return filtered;
}

void AnalyticsService::updateValue(const QString &metricId, const QDate &date, double value) {
    for (auto &m : metricsList) {
        if (m.id == metricId) {
            QString dateKey = date.toString("yyyy-MM-dd");
            m.history[dateKey] = value;
            saveData();
            return;
        }
    }
}

// --- Категорії ---

void AnalyticsService::addCategory(const QString &name) {
    if (!categoriesList.contains(name)) {
        categoriesList.append(name);
        saveData(); 
    }
}

QStringList AnalyticsService::getCategories() const {
    return categoriesList;
}

// --- Генератор (Без зміни категорій) ---

void AnalyticsService::generateMockData() {
    QDate today = QDate::currentDate();

    for (auto &metric : metricsList) {
        metric.history.clear(); 
        QString name = metric.name.toLower();

        // МИ БІЛЬШЕ НЕ ЗМІНЮЄМО metric.category ТУТ!
        // Категорія лишається такою, яку вибрав ти.

        for (int i = 30; i >= 0; --i) {
            QDate date = today.addDays(-i);
            QString dateKey = date.toString("yyyy-MM-dd");
            double val = 0.0;

            // Логіка лише підбирає правдоподібні цифри
            if (name.contains("sleep") || name.contains("сон")) {
                val = 5.0 + (QRandomGenerator::global()->generate() % 5) + ((QRandomGenerator::global()->generate() % 10) / 10.0);
            } 
            else if (name.contains("code") || name.contains("c++")) {
                val = (QRandomGenerator::global()->generate() % 10);
            }
            else if (name.contains("money") || name.contains("uah")) {
                val = 50 + (QRandomGenerator::global()->generate() % 500);
            }
            else {
                val = (QRandomGenerator::global()->generate() % 100);
            }

            metric.history.insert(dateKey, val);
        }
    }
    
    saveData();
    qDebug() << "🎲 Mock values generated (Categories preserved)!";
}

void AnalyticsService::updateMetricDetails(const QString &id, const QString &newName, 
                                           const QString &newColor, const QString &newUnits,
                                           double newMin, double newMax) {
    for (auto &m : metricsList) {
        if (m.id == id) {
            m.name = newName;
            m.color = newColor;
            m.units = newUnits;
            m.minVal = newMin;
            m.maxVal = newMax;
            saveData();
            return;
        }
    }
}

// Перейменування категорії
void AnalyticsService::renameCategory(const QString &oldName, const QString &newName) {
    if (oldName == newName || newName.isEmpty()) return;

    // 1. Оновлюємо список категорій
    for (auto &cat : categoriesList) {
        if (cat == oldName) {
            cat = newName;
            break;
        }
    }

    // 2. Оновлюємо всі метрики, що належали старій категорії
    for (auto &m : metricsList) {
        if (m.category == oldName) {
            m.category = newName;
        }
    }
    
    saveData();
}

// Видалення категорії
void AnalyticsService::deleteCategory(const QString &categoryName) {
    // 1. Видаляємо з списку
    categoriesList.removeAll(categoryName);

    // 2. Видаляємо всі метрики цієї категорії (або можна переносити в 'General', але поки видаляємо)
    auto it = std::remove_if(metricsList.begin(), metricsList.end(), 
                             [&](const Metric& m) { return m.category == categoryName; });
    metricsList.erase(it, metricsList.end());

    saveData();
}