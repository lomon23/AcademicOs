#ifndef METRICMANAGER_H
#define METRICMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QDate>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

// Типи метрик, щоб знати, який віджет малювати
enum class MetricType {
    Number,     // QSpinBox (Кількість сторінок, віджимань)
    Slider,     // QSlider (Настрій 1-10, Енергія)
    Boolean,    // QCheckBox (Пив вітаміни? Був в залі?)
    Money       // QDoubleSpinBox (Витрати)
};

// Структура, що описує саму метрику (Креслення)
struct MetricDefinition {
    QString id;         // Унікальний ключ ("mood", "pushups")
    QString name;       // Назва для людей ("Mood", "Push-ups")
    MetricType type;    // Тип
    QString unit;       // Одиниця виміру ("hours", "UAH", "/10")
    double min = 0;     // Для слайдерів/спінбоксів
    double max = 100;   
    
    // Метод для конвертації в JSON (щоб зберегти конфіг)
    QJsonObject toJson() const {
        QJsonObject json;
        json["id"] = id;
        json["name"] = name;
        json["type"] = (int)type;
        json["unit"] = unit;
        json["min"] = min;
        json["max"] = max;
        return json;
    }

    // Метод для завантаження з JSON
    static MetricDefinition fromJson(const QJsonObject &json) {
        MetricDefinition def;
        def.id = json["id"].toString();
        def.name = json["name"].toString();
        def.type = (MetricType)json["type"].toInt();
        def.unit = json["unit"].toString();
        def.min = json["min"].toDouble();
        def.max = json["max"].toDouble();
        return def;
    }
};

class MetricManager : public QObject {
    Q_OBJECT

public:
    static MetricManager& instance();

    // --- Керування Метриками ---
    // Створити нову метрику (наприклад, при створенні графіка)
    void registerMetric(const QString &id, const QString &name, MetricType type, const QString &unit = "", double min = 0, double max = 100);
    
    // Отримати список всіх метрик (для сторінки Daily Input)
    QList<MetricDefinition> getAllDefinitions() const;
    
    // Перевірити, чи існує така метрика
    bool hasMetric(const QString &id) const;

    // --- Керування Даними ---
    // Записати значення за конкретну дату
    void logValue(const QString &id, const QDate &date, const QVariant &value);
    
    // Отримати значення (якщо немає - поверне 0 або invalid)
    QVariant getValue(const QString &id, const QDate &date) const;

    // Отримати історію за діапазон (для Графіків)
    // Повертає мапу: Дата -> Значення
    QMap<QDate, double> getHistory(const QString &id, const QDate &from, const QDate &to) const;

    // --- Системні ---
    void save();
    void load();

private:
    MetricManager(QObject *parent = nullptr);
    ~MetricManager();
    MetricManager(const MetricManager&) = delete;
    MetricManager& operator=(const MetricManager&) = delete;

    // Список визначень метрик: Key = ID
    QMap<QString, MetricDefinition> definitions;
    
    // Самі дані: Key = MetricID, Value = (Key = DateString, Value = ActualValue)
    // Використовуємо QString для дати як ключ, бо JSON не любить складні ключі
    QMap<QString, QMap<QString, QVariant>> dataStore; 

    QString getFilePath() const;
};

#endif // METRICMANAGER_H