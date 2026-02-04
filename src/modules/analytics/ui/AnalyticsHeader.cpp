#include "AnalyticsHeader.h"
#include "src/modules/analytics/core/AnalyticsService.h"
#include <QPushButton>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QAction>

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
    
    // Вмикаємо Контекстне меню (ПКМ)
    btn->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(btn, &QPushButton::customContextMenuRequested, this, &AnalyticsHeader::onTabContextMenu);
    
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
        refreshTabs(); 
        // Можна додати перемикання на нову вкладку тут
    }
}

// --- ЛОГІКА КОНТЕКСТНОГО МЕНЮ ---
void AnalyticsHeader::onTabContextMenu(const QPoint &pos) {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    QString categoryName = btn->text(); // Беремо назву прямо з кнопки

    QMenu menu(this);
    menu.setStyleSheet("QMenu { background-color: #2E2E2E; color: white; border: 1px solid #555; }"
                       "QMenu::item:selected { background-color: #BD93F9; color: black; }");

    QAction *renameAction = menu.addAction("✏️ Rename");
    QAction *deleteAction = menu.addAction("🗑 Delete");

    QAction *selectedItem = menu.exec(btn->mapToGlobal(pos));

    if (selectedItem == renameAction) {
        bool ok;
        QString newName = QInputDialog::getText(this, "Rename Tab", "New Name:", 
                                                QLineEdit::Normal, categoryName, &ok);
        if (ok && !newName.isEmpty() && newName != categoryName) {
            // Викликаємо метод бекенду (ти його вже додав у AnalyticsService)
            AnalyticsService::instance().renameCategory(categoryName, newName);
            refreshTabs(); 
        }
    }
    else if (selectedItem == deleteAction) {
        auto reply = QMessageBox::question(this, "Delete Category", 
            "Delete '" + categoryName + "' and ALL its metrics?", 
            QMessageBox::Yes | QMessageBox::No);
            
        if (reply == QMessageBox::Yes) {
            // Викликаємо метод бекенду
            AnalyticsService::instance().deleteCategory(categoryName);
            refreshTabs();
            
            // Якщо після видалення є ще вкладки, вибираємо першу
            if (layout->count() > 1) { // >1 бо там ще кнопка "+"
                // Знаходимо першу кнопку (нульовий індекс)
                if (auto firstBtn = qobject_cast<QPushButton*>(layout->itemAt(0)->widget())) {
                    firstBtn->click();
                }
            } else {
                emit categorySelected(""); // Пусто
            }
        }
    }
}