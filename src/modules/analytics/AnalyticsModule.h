#ifndef ANALYTICSMODULE_H
#define ANALYTICSMODULE_H

#include <QObject>
#include <QVector>
#include "AnalyticsSmallWidget.h"
#include "AnalyticsFullPage.h"
#include "../../core/StorageManager.h" // Наш менеджер JSON

class AnalyticsModule : public QObject {
    Q_OBJECT
public:
    explicit AnalyticsModule(QObject *parent = nullptr);

    // Фабричні методи: "Дай мені віджет!"
    AnalyticsSmallWidget* createSmallWidget();
    AnalyticsFullPage* createFullPage();
    void setTitle(const QString &title);
    QString getTitle() const { return moduleTitle; }

    // Робота з даними
// Читає з JSON через StorageManager
    void saveData(); // Пише в JSON

private:
    // Тут ми зберігаємо "Правду" (дані)
    QVector<double> productivityData;
    QString moduleTitle;
    void loadData();
    // Ключ для збереження в конфігу
    const QString STORAGE_KEY = "analytics_data";
};

#endif // ANALYTICSMODULE_H