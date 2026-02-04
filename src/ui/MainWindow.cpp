#include "src/ui/MainWindow.h"
#include <QDebug>

// Core
#include "src/modules/finance/core/FinanceModule.h"
#include "src/modules/todo/core/ToDoModule.h"

// Factory
#include "src/ui/factory/PageFactory.h"

// Page Headers (потрібні, щоб робити qobject_cast або викликати методи сторінок)
#include "src/modules/dashboard/Dashboard.h"
#include "src/modules/dashboard/DailyPage.h"
#include "src/modules/finance/ui/FinanceFullPage.h"
#include "src/modules/todo/ui/ToDoPage.h" 

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setupModules(); // 1. Створюємо логіку
    setupPages();   // 2. Створюємо UI через Фабрику
    setupConnections();

    // Стартова сторінка
    if (pageMap.contains("dashboard")) {
        pagesStack->setCurrentIndex(pageMap["dashboard"]);
    }
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    this->resize(1280, 720);
    this->setWindowTitle("Academic OS");
    
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    mainLayout = new QGridLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Сайдбар
    sidebar = new Sidebar(this);
    
    // Stacked Widget (Контейнер сторінок)
    pagesStack = new QStackedWidget(centralWidget);

    // Layout: Сайдбар зліва (фікс), Сторінки справа (розтягуються)
    mainLayout->addWidget(sidebar, 0, 0);
    mainLayout->addWidget(pagesStack, 0, 1);
    mainLayout->setColumnStretch(1, 1);
}

void MainWindow::setupModules() {
    // Ініціалізація Core модулів (те, що не є UI)
    todoModule = new ToDoModule(this);
    
    // FinanceModule - це сінглтон, його не треба створювати через new, 
    // він створиться сам при першому виклику instance()
}

void MainWindow::setupPages() {
    // 🔥 Створюємо Фабрику і даємо їй залежності
    PageFactory factory(todoModule);

    // 1. Dashboard
    QWidget* dashWidget = factory.createPage("dashboard", this);
    dashboardPage = qobject_cast<Dashboard*>(dashWidget); // Зберігаємо вказівник
    registerPage("dashboard", dashWidget);

    // 2. Calendar
    registerPage("calendar", factory.createPage("calendar", this));

    // 3. Daily
    QWidget* dailyWidget = factory.createPage("daily", this);
    dailyPage = qobject_cast<DailyPage*>(dailyWidget);
    registerPage("daily", dailyWidget);

    // 4. ToDo
    registerPage("todo", factory.createPage("todo", this));

    // 5. Analytics
    registerPage("analytics", factory.createPage("analytics", this));

    // 6. Wallet
    QWidget* walletWidget = factory.createPage("wallet", this);
    financePage = qobject_cast<FinanceFullPage*>(walletWidget);
    registerPage("wallet", walletWidget);
}

void MainWindow::setupConnections() {
    // Тепер тут чисто і красиво
    connect(sidebar, &Sidebar::navigationRequested, this, &MainWindow::onNavigationChanged);
    
    if (dashboardPage) {
        connect(dashboardPage, &Dashboard::requestDailyPage, this, &MainWindow::openDailyPage);
    }
}

void MainWindow::registerPage(const QString &id, QWidget *page) {
    if (page) {
        int index = pagesStack->addWidget(page);
        pageMap[id] = index;
    }
}

void MainWindow::openDailyPage() {
    // Тут поки логіка мінімальна, бо DailyPage ще в процесі
    if (dailyPage) {
        dailyPage->refreshData(); // Якщо такий метод є
        pagesStack->setCurrentWidget(dailyPage);
    }
}
void MainWindow::onNavigationChanged(const QString &id) {
    if (id == "daily") {
        openDailyPage(); 
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
}