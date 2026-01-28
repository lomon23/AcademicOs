#include "AnalyticsHeader.h"
#include "../../core/analytics/AnalyticsService.h"
#include <QPushButton>
#include <QInputDialog>

AnalyticsHeader::AnalyticsHeader(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet("background: transparent;");
    this->setFixedHeight(50); 

    layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignLeft); // Вкладки зліва

    refreshTabs();
}

void AnalyticsHeader::refreshTabs() {
    // 1. Очищаємо старі кнопки
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    // 2. Створюємо кнопки з реальних категорій
    auto categories = AnalyticsService::instance().getCategories();
    for (const QString &cat : categories) {
        createTabButton(cat);
    }

    // 3. Додаємо кнопку "+" (New Tab)
    QPushButton *addBtn = new QPushButton("+", this);
    addBtn->setFixedSize(30, 30);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet(
        "QPushButton { background: #333; color: white; border-radius: 4px; font-weight: bold; }"
        "QPushButton:hover { background: #444; }"
    );
    connect(addBtn, &QPushButton::clicked, this, &AnalyticsHeader::onAddCategoryClicked);
    layout->addWidget(addBtn);
    
    layout->addStretch(); // Пружина в кінці
}

void AnalyticsHeader::createTabButton(const QString &name) {
    QPushButton *btn = new QPushButton(name, this);
    btn->setCheckable(true);
    btn->setAutoExclusive(true); // Лише одна кнопка може бути натиснута
    btn->setCursor(Qt::PointingHandCursor);
    
    // Стиль
    btn->setStyleSheet(
        "QPushButton { color: #888; background: transparent; border: none; font-size: 16px; font-weight: bold; padding: 5px 10px; }"
        "QPushButton:hover { color: white; }"
        "QPushButton:checked { color: #50FA7B; border-bottom: 2px solid #50FA7B; }"
    );

    connect(btn, &QPushButton::clicked, [this, name]() {
        emit categorySelected(name);
    });

    layout->addWidget(btn);
    
    // Якщо це перша вкладка - клікаємо її програмно (щоб не було пусто)
    if (layout->count() == 1) {
        btn->setChecked(true);
        emit categorySelected(name);
    }
}

void AnalyticsHeader::onAddCategoryClicked() {
    bool ok;
    QString text = QInputDialog::getText(this, "New Category",
                                         "Category Name:", QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty()) {
        AnalyticsService::instance().addCategory(text);
        refreshTabs(); // Оновлюємо кнопки
        
        // Автоматично перемикаємось на нову вкладку? (можна, але поки просто оновимо)
    }
}