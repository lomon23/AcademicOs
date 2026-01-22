#include "MainWindow.h"
#include <QLabel>
#include <QDebug>
#include <QUuid>

#include "../core/StorageManager.h"

// Modules
#include "../modules/analytics/AnalyticsSmallWidget.h"
#include "../modules/analytics/AnalyticsFullPage.h"
#include "../modules/analytics/AnalyticsModule.h"

#include "../modules/finance/FinanceModule.h"
#include "../modules/finance/FinanceFullPage.h"
#include "../modules/finance/FinanceSmallWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 1. Налаштування вікна
    this->resize(1280, 720);
    this->setWindowTitle("Academic OS");

    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    // 2. Створення лейауту
    mainLayout = new QGridLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 3. Створення Сайдбару
    sidebar = new Sidebar(this);
    mainLayout->addWidget(sidebar, 0, 0, 1, 1);

    // 4. Створення Стеку сторінок (ВАЖЛИВО: Створити ПЕРЕД registerPage)
    pagesStack = new QStackedWidget(centralWidget);
    mainLayout->addWidget(pagesStack, 0, 1, 1, 1);

    // Налаштування пропорцій (сайдбар вузький, контент широкий)
    mainLayout->setColumnStretch(0, 0);
    mainLayout->setColumnStretch(1, 1);

    // 5. Створення Дашборду і реєстрація
    dashboardPage = new Dashboard(this);
    registerPage("dashboard", dashboardPage); // Тепер pagesStack існує, все ок!

    // 6. Підключення навігації
    connect(sidebar, &Sidebar::navigationRequested, [this](const QString &id){
        if (pageMap.contains(id)) {
            pagesStack->setCurrentIndex(pageMap[id]);
        } else {
            qDebug() << "Page not found for ID:" << id;
        }
    });

    connect(dashboardPage, &Dashboard::requestWidget, this, &MainWindow::handleWidgetCreation);

    // 7. Завантаження стану
    loadDashboard();
    
    // Старт на дашборді
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
    sidebar->addButton(pageId, "Wallet", "💳");
    
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
MainWindow::~MainWindow() {

}