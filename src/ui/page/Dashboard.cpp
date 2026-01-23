#include "Dashboard.h"
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QDebug>
#include <QStringListModel>

// 👇 Не забудь перевірити шлях, якщо папки називаються інакше
#include "../components/WelcomeWidget.h" 
#include "../../core/todo/ToDoModule.h"
#include "../page/todo/ToDoSmallWidget.h"

Dashboard::Dashboard(QWidget *parent) : QWidget(parent)
{
    setupUi();
    setupSearch();
}

void Dashboard::setupUi() {
    // 1. Головний лейоут (Вертикальний)
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // --- HEADER (Кнопка Add) ---
    QWidget *header = new QWidget(this);
    header->setFixedHeight(50);
    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *title = new QLabel("Dashboard", this);
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    headerLayout->addWidget(title);

    headerLayout->addStretch(); // Пружина, щоб кнопка була справа

    QPushButton *addBtn = new QPushButton("+ Add Widget", this);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setFixedSize(120, 35);
    addBtn->setStyleSheet("QPushButton { background-color: #00E676; color: #121212; border-radius: 6px; font-weight: bold; }"
                          "QPushButton:hover { background-color: #69F0AE; }");
    connect(addBtn, &QPushButton::clicked, this, &Dashboard::onAddClicked);
    headerLayout->addWidget(addBtn);

    mainLayout->addWidget(header);
    

    // --- SEARCH BAR (Прихований за замовчуванням) ---
    searchContainer = new QWidget(this);
    searchContainer->setVisible(false); // Ховаємо на старті
    QVBoxLayout *searchLayout = new QVBoxLayout(searchContainer);
    searchLayout->setContentsMargins(0, 0, 0, 0);

    searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("Type widget name (e.g. 'Finance', 'Analytics') and press Enter...");
    searchBar->setStyleSheet(
        "QLineEdit { "
        "  background-color: #252526; " // VS Code dark input
        "  color: white; "
        "  border: 1px solid #3C3C3C; "
        "  border-radius: 4px; "
        "  padding: 8px; "
        "  font-size: 14px; "
        "}"
        "QLineEdit:focus { border: 1px solid #007FD4; }" // VS Code blue border
    );
    connect(searchBar, &QLineEdit::returnPressed, this, &Dashboard::onSearchReturnPressed);
    
    searchLayout->addWidget(searchBar);
    mainLayout->addWidget(searchContainer);


    // --- WELCOME WIDGET (НОВЕ) ---
    // Додаємо його перед сіткою, щоб він був зверху як Hero-секция
    
    WelcomeWidget *welcome = new WelcomeWidget(this);
    mainLayout->addWidget(welcome);

    // Підключаємо кнопку (поки просто виводимо в консоль)
    connect(welcome, &WelcomeWidget::startDayClicked, [this](){
        emit requestDailyPage(); // <--- Прокидаємо сигнал вгору
    });

    mainLayout->addSpacing(10); // Невеликий відступ перед сіткою
    // ----------------------------


    // --- GRID (Для віджетів) ---
    // Ми кладемо Grid у ScrollArea або просто розтягуємо
    QWidget *gridContainer = new QWidget(this);
    gridContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    gridLayout = new QGridLayout(gridContainer);
    gridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    gridLayout->setSpacing(20);
    
    mainLayout->addWidget(gridContainer);

    ToDoModule *todoMod = nullptr;
    if (todoMod) {
        ToDoSmallWidget *todoWidget = qobject_cast<ToDoSmallWidget*>(todoMod->createSmallWidget());
        
        // Клік по віджету -> відкрити сторінку "todo"
        connect(todoWidget, &ToDoSmallWidget::clicked, [this](){
            emit navigationRequested("todo"); // Треба додати цей сигнал в Dashboard
        });

        // Додаємо в сітку (ряд 0, колонка 1 - справа від привітання, наприклад)
        gridLayout->addWidget(todoWidget, 0, 1, 2, 1); 
    }
}

void Dashboard::setupSearch() {
    // Список доступних віджетів
    QStringList wordList;
    wordList << "Analytics Chart" << "Finance Wallet";

    // Налаштування автопідказки
    completer = new QCompleter(wordList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains); 
    
    // Стилізація випадаючого списку (Popup)
    QAbstractItemView *popup = completer->popup();
    popup->setStyleSheet(
        "QAbstractItemView { background: #252526; color: white; selection-background-color: #007FD4; }"
    );

    searchBar->setCompleter(completer);
}

void Dashboard::onAddClicked() {
    // Перемикаємо видимість пошуку
    bool isVisible = searchContainer->isVisible();
    searchContainer->setVisible(!isVisible);

    if (!isVisible) {
        searchBar->setFocus(); 
        searchBar->clear();
    }
}

void Dashboard::onSearchReturnPressed() {
    QString text = searchBar->text();
    if (text.isEmpty()) return;

    qDebug() << "Requesting widget creation:" << text;

    // Відправляємо сигнал у MainWindow
    emit requestWidget(text);

    // Ховаємо пошук
    searchBar->clear();
    searchContainer->setVisible(false);
}

void Dashboard::addModuleWidget(QWidget *widget) {
    if (!widget) return;
    
    // Шукаємо вільне місце в сітці
    int row = widgets.size() / 2; // 2 стовпці
    int col = widgets.size() % 2;
    
    // Додаємо в сітку
    gridLayout->addWidget(widget, row, col);
    widgets.append(widget);
}

void Dashboard::setToDoModule(ToDoModule *module) {
    todoModule = module;
    if (todoModule) {
        // Створюємо малий віджет через модуль
        QWidget *w = todoModule->createSmallWidget();
        
        // Кастимо до нашого типу, щоб підключити сигнал
        ToDoSmallWidget *smallWidget = qobject_cast<ToDoSmallWidget*>(w);
        
        if (smallWidget) {
            connect(smallWidget, &ToDoSmallWidget::clicked, [this](){
                emit navigationRequested("todo"); // Тепер це працюватиме!
            });
            
            // Додаємо в сітку (ряд 0, колонка 1 - тобто справа)
            // Або вибери інше місце в грідах
            gridLayout->addWidget(smallWidget, 2, 2, 2, 1);
        }
    }
}