#include "src/modules/todo/ui/ToDoRightBar.h"

ToDoRightBar::ToDoRightBar(QWidget *parent) : QWidget(parent) {
    setFixedWidth(300); // Фіксована ширина сайдбару
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: #1E1E1E; border-left: 1px solid #333;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 30, 20, 30);
    mainLayout->setSpacing(20);

    // Заголовок
    QLabel *title = new QLabel("Progress", this);
    title->setStyleSheet("color: #DDD; font-size: 18px; font-weight: bold;");
    mainLayout->addWidget(title);

    // Контейнер для списку барів
    contentLayout = new QVBoxLayout();
    contentLayout->setSpacing(15);
    mainLayout->addLayout(contentLayout);

    mainLayout->addStretch(); // Пружина знизу
}

void ToDoRightBar::refreshStats(ToDoModule *module) {
    // 1. Очищаємо старе
    QLayoutItem *item;
    while ((item = contentLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    if (!module) return;

    auto categories = module->getCategories();
    
    // 2. Рахуємо статистику по кожній категорії
    for (const auto &cat : categories) {
        auto tasks = module->getTasksByCategory(cat.id);
        
        int total = 0;
        int done = 0;
        
        // Рахуємо тільки "справжні" таски (можна фільтрувати підзадачі, 
        // але поки рахуємо все, що є в списку)
        for (const auto &t : tasks) {
            total++;
            if (t.isDone) done++;
        }

        // Малюємо бар, тільки якщо є таски (або показуємо 0/0)
        if (total >= 0) { 
            QWidget *progWidget = createProgressItem(cat.name, cat.color, done, total);
            contentLayout->addWidget(progWidget);
        }
    }
}

QWidget* ToDoRightBar::createProgressItem(const QString &name, const QString &color, int done, int total) {
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    // Рядок: Назва + (5/10)
    QHBoxLayout *topRow = new QHBoxLayout();
    QLabel *nameLbl = new QLabel(name);
    nameLbl->setStyleSheet("color: #BBB; font-weight: 500; font-size: 13px;");
    
    QLabel *statsLbl = new QLabel(QString("%1/%2").arg(done).arg(total));
    statsLbl->setStyleSheet("color: #666; font-size: 12px;");
    
    topRow->addWidget(nameLbl);
    topRow->addStretch();
    topRow->addWidget(statsLbl);
    layout->addLayout(topRow);

    // Прогрес бар
    QProgressBar *bar = new QProgressBar();
    bar->setFixedHeight(6);
    bar->setTextVisible(false); // Текст всередині не треба
    bar->setRange(0, total > 0 ? total : 1); // Щоб не ділити на нуль
    bar->setValue(done);
    
    // Стиль бару (динамічний колір)
    QString style = QString(
        "QProgressBar { border: none; background: #2D2D2D; border-radius: 3px; }"
        "QProgressBar::chunk { background-color: %1; border-radius: 3px; }"
    ).arg(color);
    bar->setStyleSheet(style);
    
    layout->addWidget(bar);

    return widget;
}