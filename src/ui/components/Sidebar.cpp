#include "Sidebar.h"

Sidebar::Sidebar(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedWidth(240);
    setStyleSheet("background-color: #1E1E1E; border-right: 1px solid #333;");
    
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(5); // Менший відступ між елементами
    mainLayout->setContentsMargins(15, 20, 15, 20);

    // --- 1. СЕКЦІЯ MAIN ---
    addHeader("MAIN");
    addButton("dashboard", "Dashboard", "⌂");
    addButton("daily", "Daily Check-in", "📝");
    addButton("todo", "Todo List", "☑️");
    addButton("calendar", "Calendar", "📅");
    

    mainLayout->addSpacing(15); // Відступ між секціями

    // --- 2. СЕКЦІЯ WORKSPACE ---
    addHeader("WORKSPACE");
    // (Гаманець додається динамічно, але він впаде сюди, якщо ми викличемо addButton)
    // Якщо хочеш гаманець фіксованим - розкоментуй або лиши динамічним

    // --- 3. СЕКЦІЯ ANALYTICS ---
    mainLayout->addSpacing(15);
    addHeader("ANALYTICS");

    // Створюємо контейнер для графіків
    chartsLayout = new QVBoxLayout();
    chartsLayout->setSpacing(2);
    chartsLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(chartsLayout); // Вставляємо цей контейнер в меню

    // --- 4. НИЗ (Пружина + Settings) ---
    mainLayout->addStretch();
    
    // Лінія розділювач (опціонально)
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #333;");
    mainLayout->addWidget(line);

    addButton("settings", "Settings", "⚙️");
}

// Допоміжний метод для красивих заголовків
void Sidebar::addHeader(const QString &text) {
    QLabel *header = new QLabel(text, this);
    header->setStyleSheet("color: #666; font-weight: bold; font-size: 11px; padding-left: 5px; margin-bottom: 5px;");
    mainLayout->addWidget(header);
}

// Звичайне додавання (падає в кінець списку перед пружиною, або в WORKSPACE, якщо ми це контролюємо)
void Sidebar::addButton(const QString &id, const QString &text, const QString &icon) {
    SidebarItem *btn = new SidebarItem(id, text, icon, this);
    
    connect(btn, &QPushButton::clicked, [this, id]() {
        emit navigationRequested(id);
    });

    itemsMap.insert(id, btn);

    // Логіка вставки: Settings завжди внизу, решта - по порядку
    // (Але оскільки у нас тепер складна структура, простіше додавати динамічні кнопки 
    // в chartsLayout або в mainLayout перед пружиною)
    
    // Хитрість: вставляємо перед chartsLayout? Ні.
    // Давай так: Wallet (addButton) вставляємо перед заголовком "ANALYTICS"
    // Це трохи складно вирахувати індекс, тому поки що додаємо просто в mainLayout 
    // (але в ідеалі треба мати containerWorkspace)
    
    // Спрощений варіант: Вставляємо в chartsLayout? НІ.
    // Вставляємо в mainLayout на позицію 7 (після WORKSPACE заголовка).
    // Для простоти зараз: addButton додає ПЕРЕД ANALYTICS.
    
    // Шукаємо chartsLayout в mainLayout
    int chartIndex = -1;
    for(int i=0; i < mainLayout->count(); ++i) {
        if (mainLayout->itemAt(i)->layout() == chartsLayout) {
            chartIndex = i;
            break;
        }
    }
    
    // Якщо знайшли - вставляємо перед Аналітикою (тобто в Workspace)
    if (chartIndex != -1 && id != "settings") {
        mainLayout->insertWidget(chartIndex - 1, btn); // -1 бо там ще заголовок Analytics
    } else if (id == "settings") {
        mainLayout->addWidget(btn);
    } else {
        mainLayout->insertWidget(mainLayout->count() - 2, btn);
    }
}

// НОВИЙ МЕТОД ДЛЯ ГРАФІКІВ
void Sidebar::addChartButton(const QString &id, const QString &text) {
    // Графіки мають трохи іншу іконку або без неї
    SidebarItem *btn = new SidebarItem(id, text, "∿", this);
    
    // Можна зробити шрифт трохи меншим для графіків
    btn->setStyleSheet(btn->styleSheet() + "QPushButton { font-size: 13px; color: #888; }");

    connect(btn, &QPushButton::clicked, [this, id]() {
        emit navigationRequested(id);
    });

    itemsMap.insert(id, btn);

    // Додаємо в НАШ СПЕЦІАЛЬНИЙ ЛЕЙАУТ
    chartsLayout->addWidget(btn);
}

void Sidebar::updateButtonText(const QString &id, const QString &newText) {
    if (itemsMap.contains(id)) {
        SidebarItem *btn = itemsMap[id];
        QString currentText = btn->text();
        QString iconPart = currentText.left(currentText.indexOf(" ") + 1);
        btn->setText(iconPart + " " + newText);
    }
}