#include "ToDoSmallWidget.h"
#include <QLabel>
#include <QProgressBar>
#include <QMouseEvent>

ToDoSmallWidget::ToDoSmallWidget(ToDoModule *module, QWidget *parent)
    : QWidget(parent), module(module)
{
    // 1. Стиль "Картки"
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(
        "ToDoSmallWidget { background-color: #1E1E1E; border-radius: 12px; border: 1px solid #333; }"
        "ToDoSmallWidget:hover { border: 1px solid #555; }" // Підсвітка рамки при наведенні
    );
    setCursor(Qt::PointingHandCursor);
    setFixedSize(300, 250); // Фіксований розмір (можна підлаштувати)

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);

    // 2. Заголовок
    QLabel *title = new QLabel("Tasks Overview", this);
    title->setStyleSheet("color: white; font-size: 16px; font-weight: bold; border: none; background: transparent;");
    mainLayout->addWidget(title);

    // 3. Скрол-зона
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: transparent; border: none;");
    contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(0, 0, 5, 0); // Трохи відступу для скролбару
    contentLayout->setSpacing(8);
    contentLayout->setAlignment(Qt::AlignTop);

    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);

    // Перший запуск
    refresh();
}

void ToDoSmallWidget::refresh() {
    if (!module) return;

    // Очистка
    QLayoutItem *item;
    while ((item = contentLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    auto categories = module->getCategories();
    
    // Сортування: Можна додати логіку, щоб зверху були ті, де найбільше роботи.
    // Поки виводимо як є.

    for (const auto &cat : categories) {
        auto tasks = module->getTasksByCategory(cat.id);
        int total = 0;
        int done = 0;
        for (const auto &t : tasks) {
            total++;
            if (t.isDone) done++;
        }

        if (total > 0) {
            contentLayout->addWidget(createCategoryItem(cat.name, cat.color, done, total));
        }
    }
    
    contentLayout->addStretch(); // Пружина знизу
}

QWidget* ToDoSmallWidget::createCategoryItem(const QString &name, const QString &color, int done, int total) {
    QWidget *item = new QWidget();
    item->setStyleSheet("background: transparent; border: none;");
    QVBoxLayout *layout = new QVBoxLayout(item);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    // Рядок: Назва ... 50%
    QHBoxLayout *top = new QHBoxLayout();
    QLabel *lbl = new QLabel(name);
    lbl->setStyleSheet("color: #CCC; font-size: 13px; font-weight: 500;");
    
    int percent = (total == 0) ? 0 : (done * 100 / total);
    QLabel *perc = new QLabel(QString::number(percent) + "%");
    perc->setStyleSheet("color: #666; font-size: 11px;");

    top->addWidget(lbl);
    top->addStretch();
    top->addWidget(perc);
    layout->addLayout(top);

    // Бар
    QProgressBar *bar = new QProgressBar();
    bar->setFixedHeight(4);
    bar->setTextVisible(false);
    bar->setRange(0, total);
    bar->setValue(done);
    bar->setStyleSheet(QString(
        "QProgressBar { background: #333; border-radius: 2px; }"
        "QProgressBar::chunk { background: %1; border-radius: 2px; }"
    ).arg(color));
    
    layout->addWidget(bar);
    return item;
}

void ToDoSmallWidget::mousePressEvent(QMouseEvent *event) {
    emit clicked(); // Сигнал нагору
    QWidget::mousePressEvent(event);
}