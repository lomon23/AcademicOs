#include "CategoryWidget.h"
#include <QHBoxLayout>

CategoryWidget::CategoryWidget(const ToDoCategory &category, QWidget *parent)
    : QWidget(parent), categoryId(category.id)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 10, 0, 10);
    mainLayout->setSpacing(5);

    // --- 1. ЗАГОЛОВОК КАТЕГОРІЇ ---
    QWidget *header = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(10, 0, 10, 0);
    
    // Кнопка згортання (стрілочка)
    toggleBtn = new QPushButton("▼", this);
    toggleBtn->setFixedSize(24, 24);
    toggleBtn->setCursor(Qt::PointingHandCursor);
    toggleBtn->setStyleSheet("background: transparent; color: #888; font-weight: bold; border: none;");
    connect(toggleBtn, &QPushButton::clicked, this, &CategoryWidget::toggleExpand);
    headerLayout->addWidget(toggleBtn);

    // Кольорова цятка
    QLabel *colorDot = new QLabel(this);
    colorDot->setFixedSize(10, 10);
    colorDot->setStyleSheet(QString("background-color: %1; border-radius: 5px;").arg(category.color));
    headerLayout->addWidget(colorDot);

    // Назва
    QLabel *title = new QLabel(category.name, this);
    title->setStyleSheet("font-size: 14px; font-weight: bold; color: #DDD;");
    headerLayout->addWidget(title);
    
    headerLayout->addStretch(); // Пружина вправо

    mainLayout->addWidget(header);

    // --- 2. КОНТЕЙНЕР ДЛЯ ТАСКІВ ---
    tasksContainer = new QWidget(this);
    tasksLayout = new QVBoxLayout(tasksContainer);
    tasksLayout->setContentsMargins(20, 0, 0, 0); // Відступ зліва (Tree-like structure)
    tasksLayout->setSpacing(2);
    
    mainLayout->addWidget(tasksContainer);

    // Встановлюємо початковий стан (згорнуто/розгорнуто)
    if (!category.isExpanded) {
        tasksContainer->hide();
        toggleBtn->setText("▶");
    }
}

void CategoryWidget::addTaskWidget(QWidget *taskWidget) {
    tasksLayout->addWidget(taskWidget);
}

void CategoryWidget::clearTasks() {
    QLayoutItem *item;
    while ((item = tasksLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}

void CategoryWidget::toggleExpand() {
    if (tasksContainer->isVisible()) {
        tasksContainer->hide();
        toggleBtn->setText("▶");
    } else {
        tasksContainer->show();
        toggleBtn->setText("▼");
    }
    // Тут в майбутньому можна зберігати стан expanded в JSON
}