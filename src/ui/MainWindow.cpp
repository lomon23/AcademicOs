#include "MainWindow.h"
#include <QDebug>

#include "../core/StorageManager.h"

// Modules Headers
#include "../modules/finance/FinanceModule.h"
#include "../modules/finance/FinanceFullPage.h"
#include "../modules/finance/FinanceSmallWidget.h"

// Pages Headers
#include "page/DailyPage.h"
#include "page/todo/ToDoPage.h"
#include "page/calendar/CalendarPage.h"

#include "../modules/analytics/AnalyticsPage.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 1. Налаштування вікна та сітки
    setupUI();

    // 2. Створення основних модулів (ToDo, Calendar, Daily)
    setupModules();

    // 3. Налаштування сигналів та слотів
    setupConnections();

    // 4. Відновлення стану (Finance widgets, etc.)
    loadDashboard();

    // 5. Стартова сторінка
    if (pageMap.contains("analytics")) {
        pagesStack->setCurrentIndex(pageMap["analytics"]);
    }
}

MainWindow::~MainWindow() {
    // Тут можна додати очистку пам'яті, якщо потрібно, 
    // але Qt видаляє дочірні об'єкти автоматично.
}

// ==========================================
// 1. UI SETUP
// ==========================================
void MainWindow::setupUI() {
    this->resize(1280, 720);
    this->setWindowTitle("Academic OS");
    
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    mainLayout = new QGridLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Sidebar (fixed width) vs Content (expanding)
    mainLayout->setColumnStretch(0, 0);
    mainLayout->setColumnStretch(1, 1);

    sidebar = new Sidebar(this);
    mainLayout->addWidget(sidebar, 0, 0, 1, 1);

    pagesStack = new QStackedWidget(centralWidget);
    mainLayout->addWidget(pagesStack, 0, 1, 1, 1);
}

// ==========================================
// 2. MODULES SETUP
// ==========================================
void MainWindow::setupModules() {
    // --- Dashboard ---
    dashboardPage = new Dashboard(this);
    registerPage("dashboard", dashboardPage);

    // --- Calendar ---
    CalendarPage *calendarPage = new CalendarPage(this);
    registerPage("calendar", calendarPage);

    // --- Daily Page ---
    DailyPage *dailyPage = new DailyPage(this);
    registerPage("daily", dailyPage);

    // --- ToDo Module (Core) ---
    todoModule = new ToDoModule(this);
    activeModules.append(todoModule);

    ToDoPage *todoPage = new ToDoPage(this);
    todoPage->setModule(todoModule);
    registerPage("todo", todoPage);

    AnalyticsPage *analyticsPage = new AnalyticsPage(this);
    registerPage("analytics", analyticsPage);
    // Зв'язуємо дашборд з тудушкою
    dashboardPage->setToDoModule(todoModule);
}

// ==========================================
// 3. CONNECTIONS SETUP
// ==========================================
void MainWindow::setupConnections() {
    
    // --- Navigation (Sidebar) ---
    connect(sidebar, &Sidebar::navigationRequested, [this](const QString &id){
        if (id == "daily") {
            openDailyPage(); // Спеціальна логіка для Daily
        } 
        else if (id == "todo") {
            // Оновлюємо ToDo перед показом
            if (pageMap.contains("todo")) {
                ToDoPage *page = qobject_cast<ToDoPage*>(pagesStack->widget(pageMap["todo"]));
                if (page) page->refreshData();
                pagesStack->setCurrentIndex(pageMap["todo"]);
            }
        }
        else if (pageMap.contains(id)) {
            pagesStack->setCurrentIndex(pageMap[id]);
        } 
        else {
            qDebug() << "Page not found for ID:" << id;
        }
    });

    // --- Dashboard Specifics ---
    connect(dashboardPage, &Dashboard::requestDailyPage, this, &MainWindow::openDailyPage);
    connect(dashboardPage, &Dashboard::requestWidget, this, &MainWindow::handleWidgetCreation);
    
    connect(dashboardPage, &Dashboard::navigationRequested, [this](QString pageId){
        if (pageMap.contains(pageId)) pagesStack->setCurrentIndex(pageMap[pageId]);
    });

    // --- Wallet Logic Integration ---
    // Слухаємо корекцію з Daily Page і передаємо в Finance Module
    DailyPage *dailyPage = qobject_cast<DailyPage*>(pagesStack->widget(pageMap["daily"]));
    if (dailyPage) {
    }
}

// ==========================================
// LOGIC & HELPER METHODS
// ==========================================

void MainWindow::registerPage(const QString &id, QWidget *page) {
    int index = pagesStack->addWidget(page);
    pageMap[id] = index;
}

void MainWindow::handleWidgetCreation(const QString &widgetName) {
    qDebug() << "Factory received request:" << widgetName;

    if (widgetName == "Finance Wallet") {
        createFinance();
    }
    
    // Зберігаємо зміни
    saveDashboard();
}

void MainWindow::createFinance() {
    // Перевірка на дублікат
    for (QObject *obj : activeModules) {
        if (qobject_cast<FinanceModule*>(obj)) return; 
    }

    FinanceModule *module = new FinanceModule(this);
    activeModules.append(module);
    
    // 1. Віджет на дашборд
    auto *smallWidget = module->createSmallWidget(); 
    dashboardPage->addModuleWidget(smallWidget);
    
    // 2. Сторінка
    FinanceFullPage *fullPage = module->createFullPage();
    QString pageId = "wallet";
    registerPage(pageId, fullPage);
    
    // 3. Клік по віджету відкриває сторінку
    connect(smallWidget, &FinanceSmallWidget::clicked, [this, pageId]() {
        if (pageMap.contains(pageId)) {
            pagesStack->setCurrentIndex(pageMap[pageId]);
        }
    });

    // Одразу відкриваємо
    if (pageMap.contains(pageId)) {
        pagesStack->setCurrentIndex(pageMap[pageId]);
    }
}

void MainWindow::openDailyPage() {
    // 1. Знаходимо модуль фінансів для отримання балансу
    FinanceModule *finMod = nullptr;
    for (QObject *mod : activeModules) {
        if (auto casted = qobject_cast<FinanceModule*>(mod)) {
            finMod = casted;
            break;
        }
    }

    double currentBal = finMod ? finMod->getTotalBalance() : 0.0;

    // 2. Відкриваємо та оновлюємо сторінку
    if (pageMap.contains("daily")) {
        DailyPage *page = qobject_cast<DailyPage*>(pagesStack->widget(pageMap["daily"]));
        if (page) {
        }
        pagesStack->setCurrentIndex(pageMap["daily"]);
    }
}

// ==========================================
// STATE MANAGEMENT (SAVE / LOAD)
// ==========================================

void MainWindow::saveDashboard() {
    QJsonArray modulesArray;

    for (QObject *obj : activeModules) {
        QJsonObject json;
        
        // Зберігаємо тільки відомі модулі
        if (auto *fin = qobject_cast<FinanceModule*>(obj)) {
            json["type"] = "finance";
            modulesArray.append(json);
        } 
        
        // ТУТ БУДЕ АНАЛІТИКА В МАЙБУТНЬОМУ
        // else if (...) {}
    }

    StorageManager::instance().saveConfig("dashboard_layout", modulesArray);
    qDebug() << "Dashboard saved!";
}

void MainWindow::loadDashboard() {
    QVariant data = StorageManager::instance().loadConfig("dashboard_layout");
    
    if (data.isValid()) {
        QJsonArray arr = data.toJsonArray();
        
        for (const auto &val : arr) {
            QJsonObject obj = val.toObject();
            QString type = obj["type"].toString();

            if (type == "finance") {
                createFinance();
            }
            // Сюди додамо відновлення аналітики пізніше
        }
    }
}