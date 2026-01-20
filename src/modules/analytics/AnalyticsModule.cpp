#include "AnalyticsModule.h"
#include <QRandomGenerator> // Для генерації випадкових даних
#include <QDebug>

AnalyticsModule::AnalyticsModule(QObject *parent) : QObject(parent)
{
    loadData();
    moduleTitle = "New Chart"; // Дефолтна назва
}

void AnalyticsModule::setTitle(const QString &title) {
    moduleTitle = title;
    // У майбутньому тут буде код для збереження в JSON:
    // StorageManager::instance().saveModuleTitle(this->id, title);
}

AnalyticsSmallWidget* AnalyticsModule::createSmallWidget() {
    AnalyticsSmallWidget *widget = new AnalyticsSmallWidget();
    
    // 1. Передаємо дані (щоб малювався графік)
    widget->setData(productivityData);
    
    // 2. Передаємо назву (щоб був заголовок)
    widget->setTitle(moduleTitle); 
    
    return widget;
}

AnalyticsFullPage* AnalyticsModule::createFullPage() {
    AnalyticsFullPage *page = new AnalyticsFullPage();
    
    page->setTitle(moduleTitle);      // Тепер це працює
    page->setData(productivityData);  // І це теж працює
    
    return page;
}

void AnalyticsModule::loadData()
{
    // Спробуємо завантажити з JSON
    QVariant savedData = StorageManager::instance().loadConfig(STORAGE_KEY);
    
    if (savedData.isValid()) {
        // Тут буде логіка парсингу...
    }

    // --- ГЕНЕРАЦІЯ ТЕСТОВИХ ДАНИХ ---
    if (productivityData.isEmpty()) {
        for (int i = 0; i < 30; ++i) {
            // Випадкове число від 50 до 100
            productivityData.append(QRandomGenerator::global()->bounded(50, 100));
        }
    }
}

void AnalyticsModule::saveData()
{
    // Поки заглушка.
    // StorageManager::instance().saveConfig(STORAGE_KEY, QVariant::fromValue(productivityData));
}