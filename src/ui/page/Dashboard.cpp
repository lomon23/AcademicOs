#include "Dashboard.h"
#include <QPushButton>

Dashboard::Dashboard(QWidget *parent) : QWidget(parent) {
    this->setAttribute(Qt::WA_StyledBackground, true);
    
    mainLayout = new QGridLayout(this);
    
    // --- ПОЛІРОВКА СІТКИ ---
    mainLayout->setContentsMargins(40, 40, 40, 40); // Відступи від країв вікна
    mainLayout->setSpacing(25); // Відстань між картками (повітря)
    // Вирівнювання: все збиваємо в лівий верхній кут, а не розтягуємо
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft); 

    QPushButton *btnAddWidget = new QPushButton("+ Add Widget", this);
    
    // 1. Даємо ім'я для QSS
    btnAddWidget->setObjectName("btnAddWidget"); 
    // 2. Робимо кнопку такою ж за розміром, як і віджети
    btnAddWidget->setFixedSize(300, 200); 
    
    // Прибираємо setStyleSheet звідси, бо тепер це в QSS файлі!
    // btnAddWidget->setStyleSheet(...); <--- ВИДАЛИТИ

    mainLayout->addWidget(btnAddWidget, 0, 0); // Починаємо з 0,0

    connect(btnAddWidget, &QPushButton::clicked, this, &Dashboard::widgetAdded);
}

void Dashboard::addModuleWidget(QWidget* widget) {
    // Логіка додавання:
    // Ми хочемо вставляти віджет ПЕРЕД кнопкою "+".
    
    int count = mainLayout->count(); // Скільки зараз елементів (включно з кнопкою)
    int widgetIndex = count - 1;     // Індекс для нового віджета
    
    // Розрахунок для сітки (наприклад, по 4 в ряд)
    int maxColumns = 4;
    int row = widgetIndex / maxColumns;
    int col = widgetIndex % maxColumns;
    
    // 1. Додаємо віджет на місце кнопки
    mainLayout->addWidget(widget, row, col);
    
    // 2. Пересуваємо кнопку "+" на наступну позицію
    int nextIndex = count;
    int nextRow = nextIndex / maxColumns;
    int nextCol = nextIndex % maxColumns;
    mainLayout->addWidget(this->findChild<QPushButton*>("btnAddWidget"), nextRow, nextCol);
}