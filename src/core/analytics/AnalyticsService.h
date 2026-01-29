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
    
    // --- НОВІ ПОЛЯ ---
    QString color; // HEX код (напр. "#50FA7B")
    QString units; // Одиниці (напр. "kg", "min", "pages")
    // -----------------
    
    QMap<QString, double> history;
    bool isVisible = true;

    double getValue(const QDate& date) const {
        QString dateKey = date.toString("yyyy-MM-dd");
        return history.value(dateKey, 0.0);
    }
};

class AnalyticsService {
public:
    // ... (instance, loadData, saveData без змін) ...
    static AnalyticsService& instance();
    void loadData();
    void saveData();

    // Оновлений метод створення: тепер можна передати колір і одиниці
    // (але зробимо їх необов'язковими, щоб не ламати старий код одразу)
    Metric createMetric(const QString &name, const QString &category, 
                        const QString &color = "#BD93F9", const QString &units = "");

    void deleteMetric(const QString &id);
    void updateMetricDetails(const QString &id, const QString &newName, const QString &newColor, const QString &newUnits);
    
    // ... (решта методів: getAllMetrics, getMetricsByCategory, addCategory...)
    std::vector<Metric> getAllMetrics() const;
    std::vector<Metric> getMetricsByCategory(const QString &category) const;
    void updateValue(const QString &metricId, const QDate &date, double value);
    void addCategory(const QString &name);
    QStringList getCategories() const;
    void generateMockData(); // (Якщо ти її ще не видалив)

private:
    AnalyticsService();
    QString getFilePath() const;
    std::vector<Metric> metricsList;
    QStringList categoriesList;
};

#endif // ANALYTICSSERVICE_H