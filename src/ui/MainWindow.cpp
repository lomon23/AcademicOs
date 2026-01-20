#include "MainWindow.h"
#include <QLabel>
#include "../core/StorageManager.h"
// Підключаємо хедери віджетів модуля
#include "../modules/analytics/AnalyticsSmallWidget.h"
#include "../modules/analytics/AnalyticsFullPage.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 1. Налаштування вікна
    this->resize(1920, 1080);
    this->setWindowTitle("Academic OS");

    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    mainLayout = new QGridLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
        
    // --- ВИПРАВЛЕННЯ: НЕ пиши "Sidebar *sidebar", пиши просто "sidebar" ---
    sidebar = new Sidebar(centralWidget); 
    mainLayout->addWidget(sidebar, 0, 0, 1, 1); 
    
    // Те саме тут
    pagesStack = new QStackedWidget(centralWidget);

    // І тут
    dashboardPage = new Dashboard(centralWidget);

    // СТОРІНКА 1: Calendar
    QLabel *calendarPage = new QLabel("Тут буде Календар");
    calendarPage->setAlignment(Qt::AlignCenter);
    // ... інші сторінки ...
    QLabel *todoPage = new QLabel("To-Do");
    QLabel *uniPage = new QLabel("Універ");
    QLabel *settingsPage = new QLabel("Налаштування");

    // --- ДОДАЄМО СТОРІНКИ В СТЕК ---
    pagesStack->addWidget(dashboardPage); // index 0
    pagesStack->addWidget(calendarPage);  // index 1
    pagesStack->addWidget(todoPage);      // index 2
    pagesStack->addWidget(uniPage);       // index 3
    pagesStack->addWidget(settingsPage);  // index 4

    // --- РОЗМІЩУЄМО СТЕК ---
    mainLayout->addWidget(pagesStack, 0, 1, 1, 1);

    mainLayout->setColumnStretch(0, 0); 
    mainLayout->setColumnStretch(1, 1); 

    // Connect
    connect(dashboardPage, &Dashboard::widgetAdded, this, &MainWindow::onAddWidgetClicked);
    connect(sidebar, &Sidebar::pageChanged, pagesStack, &QStackedWidget::setCurrentIndex);
}

void MainWindow::onAddWidgetClicked()
{
    // 1. Створюємо модуль (без назви, просто пустий)
    AnalyticsModule *newModule = new AnalyticsModule(this);
    activeModules.append(newModule);

    // 2. Створюємо віджети
    AnalyticsSmallWidget *smallWidget = newModule->createSmallWidget();
    dashboardPage->addModuleWidget(smallWidget);
    
    AnalyticsFullPage *fullPage = newModule->createFullPage();
    int newPageIndex = pagesStack->addWidget(fullPage);

    // 3. Додаємо в сайдбар (поки дефолтна назва)
    // Важливо: зберігаємо вказівник на кнопку, щоб потім змінювати її текст!
    // (Для цього треба буде трохи допрацювати Sidebar, але поки хай буде так)
    sidebar->addMenuButton("New Chart", newPageIndex);

    // --- ЛОГІКА ПЕРЕХОДУ ---
    connect(smallWidget, &AnalyticsSmallWidget::clicked, [this, newPageIndex]() {
        pagesStack->setCurrentIndex(newPageIndex);
    });

    // --- ЛОГІКА СИНХРОНІЗАЦІЇ (Full Page -> Small Widget) ---
    // Коли на великій сторінці змінили назву...
    connect(fullPage, &AnalyticsFullPage::configChanged, [smallWidget, newModule](const QString &newTitle){
        // 1. Оновлюємо малий віджет
        smallWidget->setTitle(newTitle);
        // 2. Оновлюємо сам модуль (щоб зберегти в JSON)
        newModule->setTitle(newTitle);
        // 3. Тут ще треба оновити текст кнопки в Сайдбарі (це наступний квест)
    });
}

MainWindow::~MainWindow(){

}