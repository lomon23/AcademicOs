#include "Sidebar.h"

Sidebar::Sidebar(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedWidth(240);
    setStyleSheet("background-color: #1E1E1E; border-right: 1px solid #333;");
    
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(15, 20, 15, 20);
    // Вирівнювання вгору, щоб кнопки не розліталися при розтягуванні вікна
    mainLayout->setAlignment(Qt::AlignTop); 

    // --- 1. СЕКЦІЯ MAIN ---
    addHeader("MAIN");
    createButton("dashboard", "Dashboard", "⌂");

    mainLayout->addSpacing(15); 

    // --- 2. СЕКЦІЯ WORKSPACE ---
    // Тут ми просто викликаємо createButton по черзі.
    // Оскільки ми вже в коді, вони стануть рівно під заголовком.
    addHeader("WORKSPACE");
    createButton("daily", "Daily Check-in", "📝");
    createButton("todo", "Tasks", "✅");   // Ось твій Todo
    createButton("wallet", "Wallet", "💳"); // Додали Wallet сюди ж!
    createButton("calendar", "Calendar", "📅");
    createButton("analytics","Analytics", "F");
    
    mainLayout->addSpacing(15);

    // --- 3. СЕКЦІЯ ANALYTICS ---
    addHeader("ANALYTICS");

    // Створюємо контейнер для графіків і додаємо його ВІДРАЗУ під заголовком
    chartsLayout = new QVBoxLayout();
    chartsLayout->setSpacing(2);
    chartsLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(chartsLayout); 

    // --- 4. НИЗ ---
    mainLayout->addStretch(); // Пружина штовхає все вгору
    
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #333;");
    mainLayout->addWidget(line);

    createButton("settings", "Settings", "⚙️");
}

void Sidebar::addHeader(const QString &text) {
    QLabel *header = new QLabel(text, this);
    header->setStyleSheet("color: #666; font-weight: bold; font-size: 11px; padding-left: 5px; margin-bottom: 5px; text-transform: uppercase;");
    mainLayout->addWidget(header);
}

// ТЕПЕР ЦЕЙ МЕТОД ПРОСТИЙ ЯК ДВЕРІ
void Sidebar::createButton(const QString &id, const QString &text, const QString &icon) {
    SidebarItem *btn = new SidebarItem(id, text, icon, this);
    
    connect(btn, &QPushButton::clicked, [this, id]() {
        emit navigationRequested(id);
    });

    itemsMap.insert(id, btn);
    
    // Просто додаємо в кінець поточного списку.
    // Оскільки ми викликаємо це в конструкторі в правильному порядку,
    // кнопка сама знайде своє місце.
    mainLayout->addWidget(btn);
}

void Sidebar::addChartButton(const QString &id, const QString &text) {
    SidebarItem *btn = new SidebarItem(id, text, "∿", this); // Хвилька для графіків
    btn->setStyleSheet(btn->styleSheet() + "QPushButton { font-size: 13px; color: #888; }");

    connect(btn, &QPushButton::clicked, [this, id]() {
        emit navigationRequested(id);
    });

    itemsMap.insert(id, btn);

    // Графіки летять у свій спеціальний контейнер
    chartsLayout->addWidget(btn);
}

void Sidebar::updateButtonText(const QString &id, const QString &newText) {
    if (itemsMap.contains(id)) {
        SidebarItem *btn = itemsMap[id];
        QString currentText = btn->text();
        // Зберігаємо іконку, міняємо текст
        QString iconPart = currentText.section(' ', 0, 0); 
        if(iconPart.isEmpty()) iconPart = "∿"; // фолбек
        
        btn->setText(iconPart + " " + newText);
    }
}