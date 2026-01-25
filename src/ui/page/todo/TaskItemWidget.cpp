#include "TaskItemWidget.h"
#include <QTimer> 

TaskItemWidget::TaskItemWidget(const ToDoTask &task, QWidget *parent)
    : QWidget(parent), m_task(task)
{
    // Головний лейаут
    mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(5, 2, 5, 2);
    mainLayout->setSpacing(10);

    // 1. Checkbox
    checkBox = new QCheckBox(this);
    checkBox->setChecked(task.isDone);
    checkBox->setCursor(Qt::PointingHandCursor);
    checkBox->setStyleSheet(
        "QCheckBox::indicator { width: 18px; height: 18px; border: 1px solid #666; border-radius: 4px; background: none; }"
        "QCheckBox::indicator:checked { background-color: #00E676; border-color: #00E676; }"
    );
    
    connect(checkBox, &QCheckBox::toggled, [this](bool checked){
        emit statusChanged(m_task.id, checked);
        QFont f = titleLabel->font();
        f.setStrikeOut(checked);
        titleLabel->setFont(f);
        titleLabel->setStyleSheet(checked ? "color: #666;" : "color: white;");
    });
    mainLayout->addWidget(checkBox);

    // 2. Title Stack (Label + Edit)
    titleLabel = new QLabel(task.title, this);
    QFont f = titleLabel->font();
    f.setPointSize(11);
    f.setStrikeOut(task.isDone);
    titleLabel->setFont(f);
    titleLabel->setStyleSheet(task.isDone ? "color: #666;" : "color: white;");
    
    titleEdit = new QLineEdit(task.title, this);
    titleEdit->hide(); 
    titleEdit->setStyleSheet("background: #222; color: white; border: 1px solid #00E676; border-radius: 4px; padding: 2px;");
    
    // 🔥 Логіка збереження посимвольно
    connect(titleEdit, &QLineEdit::textEdited, [this](const QString &text){
        m_task.title = text;
        emit textChanged(m_task.id, text); 
    });

    // 🔥 Логіка втрати фокусу або Enter
    connect(titleEdit, &QLineEdit::editingFinished, [this](){
        QString text = titleEdit->text().trimmed();
        
        if (text.isEmpty()) {
            emit deleteRequested(m_task.id);
        } else {
            titleLabel->setText(text);
            titleLabel->show();
            titleEdit->hide();
        }
    });

    mainLayout->addWidget(titleLabel, 1);
    mainLayout->addWidget(titleEdit, 1);

    // 3. Buttons
    addSubTaskBtn = new QPushButton("+", this);
    addSubTaskBtn->setFixedSize(24, 24);
    addSubTaskBtn->setCursor(Qt::PointingHandCursor);
    addSubTaskBtn->setStyleSheet("background: transparent; color: #888; border: none; font-weight: bold;");
    connect(addSubTaskBtn, &QPushButton::clicked, [this](){ emit addSubTaskRequested(m_task.id); });
    mainLayout->addWidget(addSubTaskBtn);

    deleteBtn = new QPushButton("×", this);
    deleteBtn->setFixedSize(24, 24);
    deleteBtn->setCursor(Qt::PointingHandCursor);
    deleteBtn->setStyleSheet("background: transparent; color: #FF5555; border: none; font-weight: bold; font-size: 16px;");
    connect(deleteBtn, &QPushButton::clicked, [this](){ emit deleteRequested(m_task.id); });
    mainLayout->addWidget(deleteBtn);
}

// 👇 ОСЬ ЦІ МЕТОДИ, ЯКІ БУЛИ ВТРАЧЕНІ
void TaskItemWidget::setIndentLevel(int level) {
    int indent = level * 30; // 30px відступу за кожен рівень
    mainLayout->setContentsMargins(5 + indent, 2, 5, 2);
}

void TaskItemWidget::startEditing() {
    titleLabel->hide();
    titleEdit->setText(m_task.title);
    titleEdit->show();
    titleEdit->setFocus();
    titleEdit->selectAll();
}