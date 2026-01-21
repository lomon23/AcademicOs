#include "MainWindow.h"
#include <QLabel>
#include <QDebug>
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
    // Налаштування вікна
    this->resize(1280, 720);
    this->setWindowTitle("Academic OS");

    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    mainLayout = new QGridLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
        
    sidebar = new Sidebar(centralWidget); 
    mainLayout->addWidget(sidebar, 0, 0, 1, 1); 
    
    pagesStack = new QStackedWidget(centralWidget);
    dashboardPage = new Dashboard(centralWidget);

    pagesStack->addWidget(dashboardPage); 

    mainLayout->addWidget(pagesStack, 0, 1, 1, 1);
    mainLayout->setColumnStretch(0, 0); 
    mainLayout->setColumnStretch(1, 1); 

    connect(sidebar, &Sidebar::pageChanged, pagesStack, &QStackedWidget::setCurrentIndex);
    connect(dashboardPage, &Dashboard::requestWidget, this, &MainWindow::handleWidgetCreation);
    
    // --- ЗАВАНТАЖУЄМО ЗБЕРЕЖЕНИЙ СТАН ---
    loadDashboard();
}

MainWindow::~MainWindow() {}

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
    module->setTitle(title); // Встановлюємо назву (з JSON або дефолтну)
    activeModules.append(module);

    AnalyticsSmallWidget *smallWidget = module->createSmallWidget();
    dashboardPage->addModuleWidget(smallWidget);
    
    AnalyticsFullPage *fullPage = module->createFullPage();
    int pageIdx = pagesStack->addWidget(fullPage);

    // Додаємо кнопку в меню (зберігаємо вказівник, щоб потім міняти текст, 
    // але поки Sidebar цього не вміє, тому просто додаємо)
    sidebar->addMenuButton(title, pageIdx);

    connect(smallWidget, &AnalyticsSmallWidget::clicked, [this, pageIdx]() {
        pagesStack->setCurrentIndex(pageIdx);
    });

    // Коли міняємо назву графіка -> Зберігаємо це в JSON
    connect(fullPage, &AnalyticsFullPage::configChanged, [this, smallWidget, module](const QString &newTitle){
        smallWidget->setTitle(newTitle);
        module->setTitle(newTitle);
        saveDashboard(); // <--- АВТОЗБЕРЕЖЕННЯ ПРИ ПЕРЕЙМЕНУВАННІ
    });
}

// --- СТВОРЕННЯ ФІНАНСІВ ---
void MainWindow::createFinance()
{
    for (QObject *obj : activeModules) {
        if (qobject_cast<FinanceModule*>(obj)) return; 
    }

    FinanceModule *module = new FinanceModule(this);
    activeModules.append(module);
    
    // 1. СТВОРЮЄМО МАЛИЙ ВІДЖЕТ
    // (Попередньо додай #include "../modules/finance/FinanceSmallWidget.h" зверху файлу!)
    // Якщо метод createSmallWidget не визначений в Base класі, то треба привести тип або додати його в Module.
    // Але ми тільки що додали його в FinanceModule.
    
    // УВАГА: Якщо createSmallWidget повертає конкретний тип FinanceSmallWidget*,
    // нам треба кастити його до QWidget* для Dashboard::addModuleWidget.
    
    auto *smallWidget = module->createSmallWidget(); // auto або FinanceSmallWidget*
    dashboardPage->addModuleWidget(smallWidget);
    
    // 2. СТВОРЮЄМО ПОВНУ СТОРІНКУ
    FinanceFullPage *fullPage = module->createFullPage();
    int pageIdx = pagesStack->addWidget(fullPage);
    
    sidebar->addMenuButton("Wallet", pageIdx);
    
    // 3. ЛОГІКА ПЕРЕХОДУ
    connect(smallWidget, &FinanceSmallWidget::clicked, [this, pageIdx]() {
        pagesStack->setCurrentIndex(pageIdx);
    });

    pagesStack->setCurrentIndex(pageIdx);
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