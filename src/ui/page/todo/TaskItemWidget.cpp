#include "TaskItemWidget.h"
#include <QFont>

TaskItemWidget::TaskItemWidget(const ToDoTask &task, QWidget *parent) 
    : QWidget(parent), taskId(task.id) 
{
    // 1. Лейаут (рядок)
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 8, 10, 8); // Внутрішні відступи
    layout->setSpacing(15);

    // Стиль самого контейнера (фон при наведенні)
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet(
        "TaskItemWidget { background-color: transparent; border-radius: 6px; }"
        "TaskItemWidget:hover { background-color: #2C2C2C; }" // Легка підсвітка
    );

    // 2. Чекбокс
    checkBox = new QCheckBox(this);
    checkBox->setChecked(task.isDone);
    checkBox->setCursor(Qt::PointingHandCursor);
    
    // Кастомний стиль чекбокса (Solid Green)
    checkBox->setStyleSheet(
        "QCheckBox::indicator { width: 18px; height: 18px; border: 2px solid #555; border-radius: 4px; background: transparent; }"
        "QCheckBox::indicator:checked { background-color: #00E676; border-color: #00E676; }" // Зелений квадрат
        "QCheckBox::indicator:hover { border-color: #888; }"
    );
    
    // З'єднуємо сигнал чекбокса з нашим слотом
    connect(checkBox, &QCheckBox::checkStateChanged, this, &TaskItemWidget::onCheckboxClicked);
    layout->addWidget(checkBox);

    // 3. Текст завдання
    titleLabel = new QLabel(task.title, this);
    QFont font("Segoe UI", 11);
    titleLabel->setFont(font);
    titleLabel->setStyleSheet("color: #FFFFFF; border: none; background: transparent;");
    layout->addWidget(titleLabel, 1); // 1 = розтягувати на всю ширину

    // 4. Застосовуємо початковий стиль (якщо вже виконано - закреслюємо)
    updateTextStyle(task.isDone);
}

void TaskItemWidget::onCheckboxClicked(int state) {
    bool isChecked = (state == Qt::Checked);
    updateTextStyle(isChecked);
    
    // Відправляємо сигнал нагору (в Page або CategoryWidget)
    emit statusChanged(taskId, isChecked);
}

void TaskItemWidget::updateData(const ToDoTask &task) {
    taskId = task.id;
    titleLabel->setText(task.title);
    
    // Блокуємо сигнали, щоб не викликати "statusChanged" при програмному оновленні
    bool oldState = checkBox->blockSignals(true);
    checkBox->setChecked(task.isDone);
    checkBox->blockSignals(oldState);
    
    updateTextStyle(task.isDone);
}

void TaskItemWidget::updateTextStyle(bool isDone) {
    QFont f = titleLabel->font();
    f.setStrikeOut(isDone);
    titleLabel->setFont(f);

    if (isDone) {
        titleLabel->setStyleSheet("color: #666666;"); // Сірий (Inactive)
    } else {
        titleLabel->setStyleSheet("color: #FFFFFF;"); // Білий (Active)
    }
}