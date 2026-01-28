#ifndef ANALYTICSSERVICE_H
#define ANALYTICSSERVICE_H

#include <QString>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QDate>
#include <vector>

// Структура однієї метрики
struct Metric {
    QString id;
    QString name;
    QMap<QString, double> history; // "2026-01-28" -> 10.0

    // Хелпер: отримати значення за дату (або 0, якщо немає)
    double getValue(const QDate& date) const {
        QString dateKey = date.toString("yyyy-MM-dd");
        return history.value(dateKey, 0.0);
    }
};

class AnalyticsService {
public:
    // Singleton (щоб доступ був звідусіль)
    static AnalyticsService& instance();

    // 1. Завантаження/Збереження
    void loadData();
    void saveData();
    void generateMockData();

    // 2. Управління метриками
    Metric createMetric(const QString &name);
    void deleteMetric(const QString &id);
    std::vector<Metric> getAllMetrics() const;

    // 3. Запис даних (Сьогодні я пробіг 5 км)
    void updateValue(const QString &metricId, const QDate &date, double value);

private:
    AnalyticsService(); // Приватний конструктор
    QString getFilePath() const;
    
    std::vector<Metric> metricsList;
};

#endif // ANALYTICSSERVICE_H