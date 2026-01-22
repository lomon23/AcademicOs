#include "AnalyticsModule.h"
#include "../../core/StorageManager.h"
#include <QRandomGenerator>

AnalyticsModule::AnalyticsModule(QObject *parent) : Module(parent)
{
    // Назву зберігаємо в змінну батька 'title'
    title = "New Chart"; 
    load();
}

// setTitle тут НЕ ПОТРІБЕН, він вже є в Module.h

AnalyticsSmallWidget* AnalyticsModule::createSmallWidget() {
    AnalyticsSmallWidget *widget = new AnalyticsSmallWidget();
    widget->setData(productivityData);
    widget->setTitle(title); // Беремо title з батьківського класу
    return widget;
}

AnalyticsFullPage* AnalyticsModule::createFullPage() {
    AnalyticsFullPage *page = new AnalyticsFullPage();
    page->setTitle(title);      
    page->setData(productivityData);
    return page;
}

void AnalyticsModule::load() {
    // 1. Завантаження JSON (Поки заглушка або твоя логіка)
    QVariant savedData = StorageManager::instance().loadConfig(STORAGE_KEY);
    if (savedData.isValid()) {
        // ... парсинг ...
    }

    // 2. Генерація тестових даних, якщо пусто
    if (productivityData.isEmpty()) {
        for (int i = 0; i < 30; ++i) {
            productivityData.append(QRandomGenerator::global()->bounded(50, 100));
        }
    }
}

void AnalyticsModule::save() {
    // Тут буде збереження
    // StorageManager::instance().saveConfig(STORAGE_KEY, ...);
}