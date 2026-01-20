#include "./Sidebar.h"



Sidebar::Sidebar(QWidget *parent) : QWidget(parent){
    
    this->setAttribute(Qt::WA_StyledBackground, true);
    
    this -> setFixedWidth(240);
    
    
    layout = new QVBoxLayout(this);

    layout -> setSpacing(10);
    layout -> setContentsMargins(15, 30, 15, 30);

    layout -> addWidget(createButton("Dashboard", 0));
    layout -> addWidget(createButton("Calendar", 1));
    layout -> addWidget(createButton("Notes", 2));
    layout -> addWidget(createButton("Todo", 3));

    layout->addStretch();

    layout->addWidget(createButton("Settings", 4));

};

QPushButton* Sidebar::createButton(const QString &text, int index)
{
    QPushButton *btn = new QPushButton(text, this);
    
    btn->setFixedHeight(45);
    btn->setCursor(Qt::PointingHandCursor);
    
    connect(btn, &QPushButton::clicked, [this, index]() {
        emit pageChanged(index);
    });

    return btn;
};
void Sidebar::addMenuButton(const QString &text, int index)
{
    // Використовуємо твій існуючий приватний метод createButton
    QPushButton *btn = createButton(text, index);
    
    // Додаємо в лейаут (перед розтяжкою addStretch, якщо вона є в кінці)
    // Якщо у тебе layout->addStretch() в кінці конструктора, то треба вставляти 
    // кнопку за індексом: layout->insertWidget(layout->count() - 1, btn);
    // Або просто:
    layout->addWidget(btn); 
}