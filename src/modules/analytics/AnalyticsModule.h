#ifndef ANALYTICSMODULE_H
#define ANALYTICSMODULE_H

#include "../Module.h" // Підключаємо батька
#include <QVector>
#include "AnalyticsSmallWidget.h"
#include "AnalyticsFullPage.h"

// Спадкуємось від Module
class AnalyticsModule : public Module {
    Q_OBJECT
public:
    explicit AnalyticsModule(QObject *parent = nullptr);

    AnalyticsSmallWidget* createSmallWidget();
    AnalyticsFullPage* createFullPage();

    // Перевизначаємо методи батька
    void save() override;
    void load() override;

private:
    QVector<double> productivityData;
    const QString STORAGE_KEY = "analytics_data";
};

#endif // ANALYTICSMODULE_H