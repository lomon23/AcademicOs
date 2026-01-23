#include "MainWindow.h"
#include <QLabel>
#include <QDebug>
#include <QUuid>

#include "../core/StorageManager.h"
#include "../core/todo/ToDoModule.h"
// Modules
#include "../modules/analytics/AnalyticsSmallWidget.h"
#include "../modules/analytics/AnalyticsFullPage.h"
#include "../modules/analytics/AnalyticsModule.h"
#include "../modules/finance/FinanceModule.h"
#include "../modules/finance/FinanceFullPage.h"
#include "../modules/finance/FinanceSmallWidget.h"


#include "page/DailyPage.h"
#include "page/todo/ToDoPage.h"
#include "page/calendar/CalendarPage.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 1. Налаштування вікна
    this->resize(1280, 720);
    this->setWindowTitle("Academic OS");
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    // 2. Лейаут
    mainLayout = new QGridLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->setColumnStretch(0, 0);
    mainLayout->setColumnStretch(1, 1);

    // 3. Компоненти
    sidebar = new Sidebar(this);
    mainLayout->addWidget(sidebar, 0, 0, 1, 1);

    pagesStack = new QStackedWidget(centralWidget);
    mainLayout->addWidget(pagesStack, 0, 1, 1, 1);

    // 4. Сторінки
    dashboardPage = new Dashboard(this);
    registerPage("dashboard", dashboardPage);

    CalendarPage *calendarPage = new CalendarPage(this);
    registerPage("calendar", calendarPage);
    
    DailyPage *dailyPage = new DailyPage(this); // (До речі, краще зробити dailyPage членом класу в .h, але поки ок)
    registerPage("daily", dailyPage);

    // --- TODO MODULE SETUP ---
    // 1. Створюємо модуль (Бекенд)
    ToDoModule *todoModule = new ToDoModule(this);
    activeModules.append(todoModule); // Додаємо в список активних модулів

    // 2. Створюємо сторінку (Фронтенд)
    ToDoPage *todoPage = new ToDoPage(this);
    todoPage->setModule(todoModule); // Передаємо модуль сторінці

    // 3. Реєструємо сторінку в системі навігації
    registerPage("todo", todoPage);

    // 4. Додаємо кнопку в Сайдбар // Можеш вибрати іншу іконку
    // 5. НАВІГАЦІЯ (Ось тут ми спростили!)
    
    // Сайдбар
// 6. Підключення навігації
    connect(sidebar, &Sidebar::navigationRequested, [this](const QString &id){
        if (id == "daily") {
            openDailyPage();
        } 
        else if (id == "todo") {
            // СПЕЦІАЛЬНИЙ ВИПАДОК ДЛЯ TODO
            if (pageMap.contains("todo")) {
                // Отримуємо вказівник на сторінку
                ToDoPage *page = qobject_cast<ToDoPage*>(pagesStack->widget(pageMap["todo"]));
                if (page) {
                    page->refreshData(); // <--- ОНОВЛЮЄМО ДАНІ ПЕРЕД ПОКАЗОМ
                }
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

    // Кнопка на дашборді (ТЕПЕР ЧИСТО!)
    connect(dashboardPage, &Dashboard::requestDailyPage, this, &MainWindow::openDailyPage);

    // 6. Логіка гаманця (це поки можна лишити тут, або теж винести в окремий метод saveDailyCheckIn)
    connect(dailyPage, &DailyPage::walletCorrection, [this](double diff){
        FinanceModule *finMod = nullptr;
        for (Module *mod : activeModules) {
            if (auto casted = qobject_cast<FinanceModule*>(mod)) {
                finMod = casted;
                break;
            }
        }
        if (finMod) {
            finMod->addTransaction("Daily Correction", diff, "Auto-adjustment");
            qDebug() << "✅ Wallet updated!";
        }
    });
    dashboardPage->setToDoModule(todoModule);
    connect(dashboardPage, &Dashboard::navigationRequested, [this](QString pageId){
        // Логіка перемикання сторінок (копія з сайдбару або виклик методу)
        // Найкраще: симулювати клік сайдбару або просто:
        if (pageMap.contains(pageId)) pagesStack->setCurrentIndex(pageMap[pageId]);
    });

    // 7. Інше
    connect(dashboardPage, &Dashboard::requestWidget, this, &MainWindow::handleWidgetCreation);
    loadDashboard();
    
    if (pageMap.contains("dashboard")) {
        pagesStack->setCurrentIndex(pageMap["dashboard"]);
    }
}



void MainWindow::onAddWidgetClicked() {} 

void MainWindow::handleWidgetCreation(const QString &widgetName)
{
    qDebug() << "Factory received request:" << widgetName;

    if (widgetName == "Analytics Chart") {
        createAnalytics(); // Створиться з дефолтною назвою
    } 
    else if (widgetName == "Finance Wallet") {
        createFinance();
    }
    
    // Після додавання нового віджета — зберігаємось!
    saveDashboard();
}

// --- СТВОРЕННЯ АНАЛІТИКИ ---
void MainWindow::createAnalytics(const QString &title)
{
    AnalyticsModule *module = new AnalyticsModule(this);
    module->setTitle(title);
    activeModules.append(module);

    AnalyticsSmallWidget *smallWidget = module->createSmallWidget();
    dashboardPage->addModuleWidget(smallWidget);
    
    AnalyticsFullPage *fullPage = module->createFullPage();
    
    // 1. ГЕНЕРУЄМО УНІКАЛЬНИЙ ID
    // Кожен графік повинен мати свій ключ (наприклад: "chart_1234-5678...")
    QString pageId = "chart_" + QUuid::createUuid().toString();
    registerPage(pageId, fullPage);
    sidebar->addChartButton(pageId, title); // Додаємо кнопку в сайдбар

    // 3. ЛОГІКА ПЕРЕХОДУ
    connect(smallWidget, &AnalyticsSmallWidget::clicked, [this, pageId]() {
        if (pageMap.contains(pageId)) {
            pagesStack->setCurrentIndex(pageMap[pageId]);
        }
    });

    // 4. ЛОГІКА ПЕРЕЙМЕНУВАННЯ (LIVE SYNC)
    connect(fullPage, &AnalyticsFullPage::configChanged, [this, smallWidget, module, pageId](const QString &newTitle){
        // Оновлюємо віджет на дашборді
        smallWidget->setTitle(newTitle);
        // Оновлюємо внутрішні дані модуля
        module->setTitle(newTitle);
        
        // ОНОВЛЮЄМО САЙДБАР МИТТЄВО!
        sidebar->updateButtonText(pageId, newTitle);
        
        // Зберігаємо в JSON
        saveDashboard(); 
    });
}

// --- СТВОРЕННЯ ФІНАНСІВ ---
void MainWindow::createFinance()
{
    // Перевірка, чи модуль вже існує
    for (QObject *obj : activeModules) {
        if (qobject_cast<FinanceModule*>(obj)) return; 
    }

    FinanceModule *module = new FinanceModule(this);
    activeModules.append(module);
    
    // 1. СТВОРЮЄМО МАЛИЙ ВІДЖЕТ
    auto *smallWidget = module->createSmallWidget(); 
    dashboardPage->addModuleWidget(smallWidget);
    
    // 2. СТВОРЮЄМО ПОВНУ СТОРІНКУ
    FinanceFullPage *fullPage = module->createFullPage();
    QString pageId = "wallet"; // <--- Твій ID
    
    registerPage(pageId, fullPage); // Додає в стек і в мапу
    
    // 3. ЛОГІКА ПЕРЕХОДУ
    // ТУТ БУЛА ПОМИЛКА: ми замінили pageIdx на pageId
    connect(smallWidget, &FinanceSmallWidget::clicked, [this, pageId]() {
        // Шукаємо індекс сторінки по ID в нашій мапі
        if (pageMap.contains(pageId)) {
            pagesStack->setCurrentIndex(pageMap[pageId]);
        }
    });

    // Якщо хочеш одразу відкрити гаманець після створення:
    if (pageMap.contains(pageId)) {
        pagesStack->setCurrentIndex(pageMap[pageId]);
    }
}
// --- SAVE / LOAD SYSTEM ---

void MainWindow::saveDashboard() {
    QJsonArray modulesArray;

    for (QObject *obj : activeModules) {
        QJsonObject json;
        
        // Перевіряємо, який це модуль
        if (auto *fin = qobject_cast<FinanceModule*>(obj)) {
            json["type"] = "finance";
            modulesArray.append(json);
        } 
        else if (auto *anal = qobject_cast<AnalyticsModule*>(obj)) {
            json["type"] = "analytics";
            json["title"] = anal->getTitle(); // Зберігаємо назву графіка
            modulesArray.append(json);
        }
    }

    // Записуємо в глобальний конфіг під ключем "dashboard_layout"
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
            else if (type == "analytics") {
                QString title = obj["title"].toString();
                if (title.isEmpty()) title = "New Chart";
                createAnalytics(title);
            }
        }
    }
}
void MainWindow::registerPage(const QString &id, QWidget *page) {
    int index = pagesStack->addWidget(page);
    pageMap[id] = index; // Запам'ятовуємо: "wallet_1" це індекс 5
}

void MainWindow::openDailyPage() {
    // 1. Шукаємо фінанси
    FinanceModule *finMod = nullptr;
    for (Module *mod : activeModules) {
        if (auto casted = qobject_cast<FinanceModule*>(mod)) {
            finMod = casted;
            break;
        }
    }

    // 2. Беремо баланс
    double currentBal = 0.0;
    if (finMod) {
        currentBal = finMod->getTotalBalance();
    }

    // 3. Відкриваємо сторінку
    if (pageMap.contains("daily")) {
        DailyPage *page = qobject_cast<DailyPage*>(pagesStack->widget(pageMap["daily"]));
        if (page) {
            page->setWalletBalance(currentBal);
            page->prepareForShow(); // <--- ОСЬ ВОНО, ОНОВЛЕННЯ ФОРМИ!
        }
        pagesStack->setCurrentIndex(pageMap["daily"]);
    }
}

MainWindow::~MainWindow() {

}
