#include "MainWindow.h"
#include "components/Sidebar.h"
#include <QStackedWidget>
#include <QLabel>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 1. Налаштування вікна
    this->resize(1920, 1080);
    this->setWindowTitle("Academic OS");


    // --- ВИПРАВЛЕННЯ 1: Створюємо основу (без цього був Segfault) ---
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);
    // -------------------------------------------------------------

    mainLayout = new QGridLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
        
    Sidebar *sidebar = new Sidebar(centralWidget);
    // Додаємо сайдбар у сітку
    mainLayout->addWidget(sidebar, 0, 0, 1, 1); 
    
    QStackedWidget *pagesStack = new QStackedWidget(centralWidget);

    QWidget *dashboardPage = new QWidget();

    
    // СТОРІНКА 1: Calendar
    QLabel *calendarPage = new QLabel("Тут буде Календар");
    calendarPage->setAlignment(Qt::AlignCenter);
   
    // СТОРІНКА 2: To-Do
    QLabel *todoPage = new QLabel("Тут буде список завдань");
    todoPage->setAlignment(Qt::AlignCenter);
 

    // СТОРІНКА 3: University
    QLabel *uniPage = new QLabel("Універ");
    uniPage->setAlignment(Qt::AlignCenter);


    // СТОРІНКА 4: Settings
    QLabel *settingsPage = new QLabel("Налаштування");
    settingsPage->setAlignment(Qt::AlignCenter);


    // --- ДОДАЄМО СТОРІНКИ В СТЕК ---
    pagesStack->addWidget(dashboardPage); // index 0
    pagesStack->addWidget(calendarPage);  // index 1
    pagesStack->addWidget(todoPage);      // index 2
    pagesStack->addWidget(uniPage);       // index 3
    pagesStack->addWidget(settingsPage);  // index 4

    // --- РОЗМІЩУЄМО СТЕК В СІТЦІ ---
    mainLayout->addWidget(pagesStack, 0, 1, 1, 1);

    // Налаштування пропорцій
    mainLayout->setColumnStretch(0, 0); // Авто
    mainLayout->setColumnStretch(1, 1); // Розтягується

    // --- ВИПРАВЛЕННЯ 2: pageChanged з маленької літери + стоїть в кінці ---
    connect(sidebar, &Sidebar::pageChanged, pagesStack, &QStackedWidget::setCurrentIndex);
}

MainWindow::~MainWindow(){
    
}