#ifndef ANALYTICSSERVICE_H
#define ANALYTICSSERVICE_H

#include <QString>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QDate>
#include <vector>
#include <QStringList> // Важливо для списків

struct Metric {
    QString id;
    QString name;
    QString category; 
    QMap<QString, double> history;
    bool isVisible = true;

    double getValue(const QDate& date) const {
        QString dateKey = date.toString("yyyy-MM-dd");
        return history.value(dateKey, 0.0);
    }
};

class AnalyticsService {
public:
    static AnalyticsService& instance();

    // Основні методи
    void loadData();
    void saveData();

    // Робота з Метриками
    Metric createMetric(const QString &name, const QString &category);
    void deleteMetric(const QString &id);
    std::vector<Metric> getAllMetrics() const;
    std::vector<Metric> getMetricsByCategory(const QString &category) const;
    void updateValue(const QString &metricId, const QDate &date, double value);
    
    // Dev Tool
    void generateMockData();

    // --- Робота з Категоріями (Tabs) ---
    void addCategory(const QString &name);
    QStringList getCategories() const;

private:
    AnalyticsService();
    QString getFilePath() const;
    
    std::vector<Metric> metricsList;
    QStringList categoriesList; // Список вкладок
};

#endif // ANALYTICSSERVICE_H