#include "src/ui/MainWindow.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "../core/StorageManager.h"

// Modules Headers
#include "src/modules/finance/core/FinanceModule.h"
#include "src/modules/finance/ui/FinanceFullPage.h"
#include "src/modules/finance/widgets/FinanceSmallWidget.h"

// Pages Headers
#include "src/modules/dashboard/DailyPage.h"
#include "src/modules/todo/ui/ToDoPage.h"
#include "src/modules/calendar/ui/CalendarPage.h"
#include "src/modules/analytics/ui/AnalyticsPage.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 1. Налаштування вікна
    setupUI();

    // 2. Створення модулів (ToDo, Calendar, Daily)
    setupModules();

    // 3. Сигнали
    setupConnections();

    // 4. Відновлення (поки не критично)
    loadDashboard();

    // 5. Стартова сторінка (Dashboard за замовчуванням)
    if (pageMap.contains("dashboard")) {
        pagesStack->setCurrentIndex(pageMap["dashboard"]);
    }
}

MainWindow::~MainWindow() {}

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
    
    mainLayout->setColumnStretch(0, 0);
    mainLayout->setColumnStretch(1, 1);

    sidebar = new Sidebar(this);
    mainLayout->addWidget(sidebar, 0, 0, 1, 1);

    pagesStack = new QStackedWidget(centralWidget);
    mainLayout->addWidget(pagesStack, 0, 1, 1, 1);
}

// ==========================================
// 2. MODULES SETUP (ВАЖЛИВІ ФІКСИ ТУТ)
// ==========================================
void MainWindow::setupModules() {
    // --- Dashboard ---
    dashboardPage = new Dashboard(this);
    registerPage("dashboard", dashboardPage);

    // --- Calendar ---
    CalendarPage *calendarPage = new CalendarPage(this);
    registerPage("calendar", calendarPage);

    // --- Daily Page ---
    // ПОМИЛКА БУЛА ТУТ: Треба використовувати змінну класу dailyPage, а не створювати нову
    dailyPage = new DailyPage(this); 
    registerPage("daily", dailyPage);

    // --- ToDo Module ---
    todoModule = new ToDoModule(this);
    activeModules.append(todoModule);

    ToDoPage *todoPage = new ToDoPage(this);
    todoPage->setModule(todoModule);
    registerPage("todo", todoPage);

    // --- Analytics ---
    AnalyticsPage *analyticsPage = new AnalyticsPage(this);
    registerPage("analytics", analyticsPage);

    // --- Wallet (Finance) ---
    financePage = new FinanceFullPage(this); 
    registerPage("wallet", financePage);     
    
    // Зв'язок
    dashboardPage->setToDoModule(todoModule);
}


// ==========================================
// 3. CONNECTIONS SETUP
// ==========================================
void MainWindow::setupConnections() {
    
    // --- Navigation (Sidebar) ---
    connect(sidebar, &Sidebar::navigationRequested, [this](const QString &id){
        if (id == "daily") {
            openDailyPage(); 
        } 
        else if (id == "wallet") {
            // Прямий перехід на сторінку Wallet
            if (financePage) {
                pagesStack->setCurrentWidget(financePage);
            }
        }
        else if (id == "todo") {
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
    
    // --- Wallet Logic Integration ---
    // (Тут поки нічого, бо ми тимчасово відключили логіку фінансів в DailyPage)
}

// ==========================================
// LOGIC & HELPER METHODS
// ==========================================

void MainWindow::registerPage(const QString &id, QWidget *page) {
    int index = pagesStack->addWidget(page);
    pageMap[id] = index;
}

void MainWindow::handleWidgetCreation(const QString &widgetName) {
    // Цей метод можна поки спростити або прибрати, якщо ми створюємо financePage в setupModules
    if (widgetName == "Finance Wallet" && !financePage) {
        createFinance();
    }
}

void MainWindow::createFinance() {
    // Цей метод тепер дублює логіку з setupModules, 
    // але залишимо як "fallback"
    if (!financePage) {
        financePage = new FinanceFullPage(this);
        pagesStack->addWidget(financePage);
        // Треба оновити pageMap, якщо ми створюємо динамічно
        registerPage("wallet", financePage);
    }
}

void MainWindow::openDailyPage() {
    // Отримуємо баланс
    double currentBal = FinanceModule::instance().getTotalBalanceInUAH();
    
    // ❌ ТИМЧАСОВО КОМЕНТУЄМО, поки не оновимо DailyPage:
    // if (dailyPage) dailyPage->setExpectedBalance(currentBal); 
    
    if (dailyPage) {
        dailyPage->refreshData();
        pagesStack->setCurrentWidget(dailyPage);
    }
}

// ==========================================
// STATE MANAGEMENT
// ==========================================

void MainWindow::saveDashboard() {
    // (Заглушка, щоб не ламалось)
}

void MainWindow::loadDashboard() {
    // (Заглушка)
}