#include "TaskItemWidget.h"
#include <QFont>

TaskItemWidget::TaskItemWidget(const ToDoTask &task, QWidget *parent) 
    : QWidget(parent), taskId(task.id) 
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- Верхній рядок ---
    QWidget *topRow = new QWidget(this);
    topRow->setAttribute(Qt::WA_StyledBackground, true);
    topRow->setStyleSheet(
        "QWidget { background-color: transparent; border-radius: 6px; }"
        "QWidget:hover { background-color: #2C2C2C; }"
    );
    
    QHBoxLayout *rowLayout = new QHBoxLayout(topRow);
    rowLayout->setContentsMargins(10, 8, 5, 8);
    rowLayout->setSpacing(10);

    // 1. Чекбокс
    checkBox = new QCheckBox(topRow);
    checkBox->setChecked(task.isDone);
    checkBox->setCursor(Qt::PointingHandCursor);
    checkBox->setStyleSheet(
        "QCheckBox::indicator { width: 18px; height: 18px; border: 2px solid #555; border-radius: 4px; background: transparent; }"
        "QCheckBox::indicator:checked { background-color: #00E676; border-color: #00E676; }"
    );
    connect(checkBox, &QCheckBox::checkStateChanged, this, &TaskItemWidget::onCheckboxClicked);
    rowLayout->addWidget(checkBox);

    // 2. Текст (Label) - Видимий спочатку
    titleLabel = new QLabel(task.title, topRow);
    QFont font("Segoe UI", 11);
    titleLabel->setFont(font);
    titleLabel->setStyleSheet("color: #FFFFFF; border: none; background: transparent;");
    rowLayout->addWidget(titleLabel, 1);

    // 3. Поле вводу (LineEdit) - Приховане спочатку
    titleEdit = new QLineEdit(topRow);
    titleEdit->setText(task.title);
    titleEdit->setFont(font);
    titleEdit->setStyleSheet("QLineEdit { color: white; background: #333; border: 1px solid #555; border-radius: 4px; padding: 2px; }");
    titleEdit->setVisible(false); // Ховаємо
    connect(titleEdit, &QLineEdit::returnPressed, this, &TaskItemWidget::onSaveClicked); // Enter зберігає
    rowLayout->addWidget(titleEdit, 1);

    // 4. Кнопка "Редагувати" (Олівець ✏️)
    editBtn = new QPushButton("✏️", topRow);
    editBtn->setFixedSize(24, 24);
    editBtn->setCursor(Qt::PointingHandCursor);
    editBtn->setToolTip("Rename");
    editBtn->setStyleSheet("QPushButton { color: #888; background: transparent; border: none; } QPushButton:hover { color: #FFF; background: #333; border-radius: 4px; }");
    connect(editBtn, &QPushButton::clicked, this, &TaskItemWidget::onEditClicked);
    rowLayout->addWidget(editBtn);

    // 5. Кнопка "Додати підзадачу" (+)
    // 🔥 ФІКС ВКЛАДЕНОСТІ: Створюємо кнопку ТІЛЬКИ якщо це не підзадача (parentTaskId пустий)
    if (task.parentTaskId.isEmpty()) {
        addSubTaskBtn = new QPushButton("+", topRow);
        addSubTaskBtn->setFixedSize(24, 24);
        addSubTaskBtn->setCursor(Qt::PointingHandCursor);
        addSubTaskBtn->setToolTip("Add Sub-task");
        addSubTaskBtn->setStyleSheet("QPushButton { color: #888; background: transparent; border: none; font-weight: bold; } QPushButton:hover { color: #00E676; background: #333; border-radius: 4px; }");
        connect(addSubTaskBtn, &QPushButton::clicked, [this](){
            emit addSubTaskRequested(taskId);
        });
        rowLayout->addWidget(addSubTaskBtn);
    } else {
        addSubTaskBtn = nullptr; // Щоб не звертатись до null
    }

    // 6. Кнопка "Видалити" (🗑️)
    deleteBtn = new QPushButton("🗑️", topRow);
    deleteBtn->setFixedSize(24, 24);
    deleteBtn->setCursor(Qt::PointingHandCursor);
    deleteBtn->setStyleSheet("QPushButton { color: #888; background: transparent; border: none; } QPushButton:hover { color: #FF5252; background: #333; border-radius: 4px; }");
    connect(deleteBtn, &QPushButton::clicked, [this](){
        emit deleteRequested(taskId);
    });
    rowLayout->addWidget(deleteBtn);

    mainLayout->addWidget(topRow);

    // --- Діти ---
    childrenContainer = new QWidget(this);
    childrenLayout = new QVBoxLayout(childrenContainer);
    childrenLayout->setContentsMargins(30, 0, 0, 0);
    childrenLayout->setSpacing(2);
    mainLayout->addWidget(childrenContainer);
    childrenContainer->setVisible(false);

    updateTextStyle(task.isDone);
}

void TaskItemWidget::addChildTask(QWidget* childWidget) {
    childrenContainer->setVisible(true);
    childrenLayout->addWidget(childWidget);
}

void TaskItemWidget::onCheckboxClicked(int state) {
    bool isChecked = (state == Qt::Checked);
    updateTextStyle(isChecked);
    emit statusChanged(taskId, isChecked);
}

// --- ЛОГІКА РЕДАГУВАННЯ ---

void TaskItemWidget::onEditClicked() {
    if (!isEditMode) {
        // Входимо в режим редагування
        toggleEditMode(true);
        titleEdit->setFocus(); // Фокус на поле вводу
    } else {
        // Зберігаємо (якщо ще раз натиснув на кнопку, яка тепер стала "зберегти")
        onSaveClicked();
    }
}

void TaskItemWidget::onSaveClicked() {
    QString newText = titleEdit->text().trimmed();
    if (!newText.isEmpty() && newText != titleLabel->text()) {
        titleLabel->setText(newText);
        emit renameRequested(taskId, newText);
    }
    toggleEditMode(false); // Виходимо з режиму
}

void TaskItemWidget::toggleEditMode(bool enable) {
    isEditMode = enable;
    
    titleLabel->setVisible(!enable); // Ховаємо текст
    titleEdit->setVisible(enable);   // Показуємо інпут
    
    if (enable) {
        editBtn->setText("💾"); // Іконка дискети
        editBtn->setStyleSheet("QPushButton { color: #00E676; background: transparent; border: none; }");
    } else {
        editBtn->setText("✏️"); // Іконка олівця
        editBtn->setStyleSheet("QPushButton { color: #888; background: transparent; border: none; } QPushButton:hover { color: #FFF; background: #333; border-radius: 4px; }");
    }
}

void TaskItemWidget::updateData(const ToDoTask &task) {
    taskId = task.id;
    titleLabel->setText(task.title);
    titleEdit->setText(task.title); // Оновлюємо і прихований інпут
    
    bool oldState = checkBox->blockSignals(true);
    checkBox->setChecked(task.isDone);
    checkBox->blockSignals(oldState);
    
    updateTextStyle(task.isDone);
}

void TaskItemWidget::updateTextStyle(bool isDone) {
    QFont f = titleLabel->font();
    f.setStrikeOut(isDone);
    titleLabel->setFont(f);
    titleLabel->setStyleSheet(isDone ? "color: #666666;" : "color: #FFFFFF;");
}