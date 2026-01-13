#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 1. Налаштування самого вікна
    this->resize(1200, 800);
    this->setWindowTitle("Academic OS");
    
    // Щоб вікно було темним (тимчасово, поки не підключили стилі)
    this->setStyleSheet("background-color: #121212;");

    // 2. Створення центрального віджета (основи)
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    // 3. Створення сітки (Layout)
    mainLayout = new QGridLayout(centralWidget);
    mainLayout->setSpacing(20); // Відступи між картками (як gap в CSS)
    mainLayout->setContentsMargins(20, 20, 20, 20); // Відступи від країв вікна

    // 4. Додаємо тестові "картки" (щоб бачити структуру)
    // Це просто кнопки для прикладу, потім замінимо на красиві QFrame
    
    // Картка 1: Сайдбар (ліва колонка, розтягується на всю висоту)
    QPushButton *sidebar = new QPushButton("Sidebar");
    sidebar->setStyleSheet("background-color: #1E1E1E; border: none; border-radius: 16px; color: white;");
    sidebar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // Додаємо в сітку: (row=0, col=0, rowSpan=2, colSpan=1)
    mainLayout->addWidget(sidebar, 0, 0, 2, 1);

    // Картка 2: Головний графік (верх, широкий)
    QPushButton *mainChart = new QPushButton("Main Chart");
    mainChart->setStyleSheet("background-color: #1E1E1E; border: none; border-radius: 16px; color: white;");
    mainChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(mainChart, 0, 1, 1, 2); // Займає 2 колонки

    // Картка 3: Календар
    QPushButton *calendar = new QPushButton("Calendar");
    calendar->setStyleSheet("background-color: #1E1E1E; border: none; border-radius: 16px; color: white;");
    calendar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(calendar, 1, 1, 1, 1);

    // Картка 4: Список завдань
    QPushButton *todo = new QPushButton("To-Do List");
    todo->setStyleSheet("background-color: #1E1E1E; border: none; border-radius: 16px; color: white;");
    todo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(todo, 1, 2, 1, 1);

    // Налаштування пропорцій колонок (ліва вужча, праві ширші)
    mainLayout->setColumnStretch(0, 1); // Sidebar вузький
    mainLayout->setColumnStretch(1, 3); // Контент широкий
    mainLayout->setColumnStretch(2, 3); // Контент широкий
}

MainWindow::~MainWindow()
{
    // У Qt не треба вручну видаляти дочірні віджети (кнопки, леяути),
    // якщо вони прикріплені до parent (this). Вони видаляться самі.
}