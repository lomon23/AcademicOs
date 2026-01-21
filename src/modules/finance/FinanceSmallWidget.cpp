#include "FinanceSmallWidget.h"
#include <QMouseEvent>
#include <QFrame>

FinanceSmallWidget::FinanceSmallWidget(FinanceModule *mod, QWidget *parent)
    : QWidget(parent), module(mod)
{
    setCursor(Qt::PointingHandCursor);
    
    // ВАЖЛИВО: Дозволяє віджету мати свій фон при використанні QSS
    setAttribute(Qt::WA_StyledBackground, true); 
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // --- СТИЛІЗАЦІЯ КАРТКИ (CSS) ---
    setStyleSheet(
        "FinanceSmallWidget { "
        "  background-color: #1E1E1E; "       // Колір картки
        "  border: 1px solid #333; "          // Тонка рамка
        "  border-radius: 16px; "             // Скруглення (як у Analytics)
        "}"
        "FinanceSmallWidget:hover { "
        "  background-color: #252525; "       // Ефект при наведенні
        "  border: 1px solid #444; "
        "}"
        "QLabel { border: none; background: transparent; }" // Щоб лейбли не мали рамок
    );

    setupUi();
    updateUI();

    connect(module, &FinanceModule::dataChanged, this, &FinanceSmallWidget::updateUI);
}

void FinanceSmallWidget::setupUi() {
    // Основний вертикальний лейоут
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20); // Внутрішні відступи (padding)
    mainLayout->setSpacing(10); // Відстань між блоками

    // 1. Верхній рядок: Заголовок + Іконка
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    QLabel *title = new QLabel("TOTAL BALANCE", this);
    title->setStyleSheet("color: #888; font-size: 11px; font-weight: bold; letter-spacing: 1px;");
    headerLayout->addWidget(title);
    
    headerLayout->addStretch();
    
    QLabel *icon = new QLabel("💳", this); // Можна замінити на іконку
    icon->setStyleSheet("font-size: 16px; color: #666;");
    headerLayout->addWidget(icon);
    
    mainLayout->addLayout(headerLayout);

    // 2. Велика цифра (Баланс)
    balanceLabel = new QLabel("0 ₴", this);
    // Використовуємо шрифт Inter або Segoe UI, великий розмір
    balanceLabel->setStyleSheet("color: #FFF; font-size: 36px; font-weight: bold; margin-bottom: 5px;");
    mainLayout->addWidget(balanceLabel);

    // Розділювач (Тонка лінія)
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: #333; max-height: 1px; border: none;");
    mainLayout->addWidget(line);
    
    // 3. Контейнер для списку (Останні витрати)
    goalsLayout = new QVBoxLayout();
    goalsLayout->setSpacing(8); // Відстань між рядками списку
    goalsLayout->setContentsMargins(0, 5, 0, 0);
    mainLayout->addLayout(goalsLayout);
    
    mainLayout->addStretch(); // Пружина, щоб контент був зверху, якщо місця багато
}

void FinanceSmallWidget::updateUI() {
    // 1. Оновлюємо баланс
    balanceLabel->setText(QString::number(module->getTotalBalance()) + " ₴");

    // 2. Оновлюємо список
    QLayoutItem *child;
    while ((child = goalsLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    const auto &list = module->getAllocations();
    
    int count = 0;
    // Показуємо останні 3
    for (auto it = list.rbegin(); it != list.rend(); ++it) {
        if (count >= 3) break;
        
        const Allocation &alloc = *it;
        
        QWidget *row = new QWidget();
        QHBoxLayout *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->setSpacing(10);

        // Крапка (Indicator)
        QLabel *dot = new QLabel("●");
        dot->setStyleSheet(QString("color: %1; font-size: 10px; margin-top: 2px;").arg(getColorForType(alloc.type)));
        rowLayout->addWidget(dot);

        // Назва
        QLabel *name = new QLabel(alloc.name);
        name->setStyleSheet("color: #CCC; font-size: 13px; font-weight: 500;");
        rowLayout->addWidget(name, 1);

        // Сума
        QLabel *amount = new QLabel(QString::number(alloc.amount) + " ₴");
        amount->setStyleSheet("color: #FFF; font-size: 13px; font-weight: bold;");
        rowLayout->addWidget(amount);

        goalsLayout->addWidget(row);
        count++;
    }
    
    // Якщо список пустий - покажемо заглушку
    if (count == 0) {
        QLabel *emptyLabel = new QLabel("No active goals", this);
        emptyLabel->setStyleSheet("color: #555; font-style: italic; font-size: 12px;");
        goalsLayout->addWidget(emptyLabel);
    }
}

QString FinanceSmallWidget::getColorForType(AllocationType type) {
    switch (type) {
        case AllocationType::Debt: return "#FF5252";     
        case AllocationType::Reserved: return "#FFD740"; 
        case AllocationType::Goal: return "#00E676";     
        default: return "#FFFFFF";
    }
}

void FinanceSmallWidget::mousePressEvent(QMouseEvent *event) {
    emit clicked();
    QWidget::mousePressEvent(event);
}