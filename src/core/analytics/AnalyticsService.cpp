#include "AnalyticsService.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>
#include <QUuid>
#include <QDebug>
#include <QRandomGenerator> // Додай це зверху

// =========================================================
// Singleton & Setup
// =========================================================

AnalyticsService& AnalyticsService::instance() {
    static AnalyticsService instance;
    return instance;
}

AnalyticsService::AnalyticsService() {
    loadData(); // Завантажуємо дані при старті програми
}

QString AnalyticsService::getFilePath() const {
    // Зберігаємо поруч з іншими даними, але в окремому файлі analytics.json
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return path + "/analytics.json";
}

// =========================================================
// Save / Load System (JSON Magic)
// =========================================================

void AnalyticsService::saveData() {
    QJsonArray metricsArray;

    // 1. Проходимось по всіх метриках у пам'яті
    for (const auto &metric : metricsList) {
        QJsonObject metricObj;
        metricObj["id"] = metric.id;
        metricObj["name"] = metric.name; // <--- Ось тут зберігається назва, яку дав користувач

        // 2. Серіалізуємо історію (Дати і Значення)
        QJsonObject historyObj;
        QMapIterator<QString, double> i(metric.history);
        while (i.hasNext()) {
            i.next();
            // JSON любить рядки як ключі, тому дата - це рядок "2026-01-28"
            historyObj[i.key()] = i.value();
        }
        metricObj["values"] = historyObj;

        metricsArray.append(metricObj);
    }

    QJsonObject root;
    root["metrics"] = metricsArray;

    // 3. Записуємо у файл
    QFile file(getFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
        file.close();
        qDebug() << "📊 Analytics saved to:" << getFilePath();
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

    metricsList.clear();
    QJsonArray metricsArray = doc.object()["metrics"].toArray();

    // Розпарсуємо JSON назад у C++ структури
    for (const auto &val : metricsArray) {
        QJsonObject obj = val.toObject();
        Metric m;
        m.id = obj["id"].toString();
        m.name = obj["name"].toString();

        QJsonObject historyObj = obj["values"].toObject();
        for (auto it = historyObj.begin(); it != historyObj.end(); ++it) {
            m.history.insert(it.key(), it.value().toDouble());
        }

        metricsList.push_back(m);
    }
    qDebug() << "📊 Loaded" << metricsList.size() << "metrics.";
}

// =========================================================
// Public Logic
// =========================================================

// Створення нової метрики (Наприклад: "Sleep", "Run")
Metric AnalyticsService::createMetric(const QString &name) {
    Metric m;
    m.id = QUuid::createUuid().toString(); // Генеруємо унікальний ID (наприклад {ab34-....})
    m.name = name;                         // Зберігаємо те, що ввів юзер
    
    metricsList.push_back(m);
    saveData(); // Одразу зберігаємо зміни
    return m;
}

void AnalyticsService::deleteMetric(const QString &id) {
    // Видаляємо метрику зі списку за ID (lambda function c++)
    metricsList.erase(std::remove_if(metricsList.begin(), metricsList.end(),
                                     [&id](const Metric& m) { return m.id == id; }),
                      metricsList.end());
    saveData();
}

std::vector<Metric> AnalyticsService::getAllMetrics() const {
    return metricsList;
}

// Оновлення даних (Коли юзер ввів дані за сьогодні)
void AnalyticsService::updateValue(const QString &metricId, const QDate &date, double value) {
    for (auto &m : metricsList) {
        if (m.id == metricId) {
            QString dateKey = date.toString("yyyy-MM-dd");
            m.history[dateKey] = value; // Записуємо або перезаписуємо значення
            saveData();
            qDebug() << "Updated metric:" << m.name << "Date:" << dateKey << "Val:" << value;
            return;
        }
    }
}

void AnalyticsService::generateMockData() {
    QDate today = QDate::currentDate();

    for (auto &metric : metricsList) {
        metric.history.clear(); // Чистимо старе
        
        QString name = metric.name.toLower();
        qDebug() << "🎲 Generating for:" << name;
        // Логіка генерації залежно від типу
        for (int i = 30; i >= 0; --i) {
            QDate date = today.addDays(-i);
            QString dateKey = date.toString("yyyy-MM-dd");
            double val = 0.0;

            // 1. СОН (Sleep) - Години (4..10)
            if (name.contains("sleep") || name.contains("сон") || name.contains("спав")) {
                val = 5.0 + (QRandomGenerator::global()->generate() % 5) + ((QRandomGenerator::global()->generate() % 10) / 10.0);
            } 
            // 2. НАСТРІЙ (Mood) - Оцінка (1..10)
            else if (name.contains("mood") || name.contains("настрій")) {
                val = 1.0 + (QRandomGenerator::global()->generate() % 10);
            }
            // 3. КРОКИ (Steps) - Тисячі (2000..15000)
            else if (name.contains("step") || name.contains("крок")) {
                val = 2000 + (QRandomGenerator::global()->generate() % 13000);
            }
            // 4. КОД (Code) - Години (0..12)
            else if (name.contains("code") || name.contains("код") || name.contains("c++")) {
                val = (QRandomGenerator::global()->generate() % 10) + ((QRandomGenerator::global()->generate() % 10) / 10.0);
            }
            // 5. ДЕФОЛТ (Інше) - (0..100)
            else {
                val = (QRandomGenerator::global()->generate() % 100);
            }

            metric.history.insert(dateKey, val);
        }
    }
    
    saveData();
    qDebug() << "🎲 Smart Mock data generated!";
}