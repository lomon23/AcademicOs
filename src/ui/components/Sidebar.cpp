#include "./Sidebar.h"


Sidebar::Sidebar(QWidget *parent) : QWidget(parent){
    this -> setFixedWidth(240);

    this -> setStyleSheet("background-color: #0d0d0dff; border-radius: 16px;");
    
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
    btn->setStyleSheet(
        "QPushButton {"
        "   color: #A0A0A0;"
        "   background-color: transparent;"
        "   border: none;"
        "   text-align: left;"
        "   padding-left: 20px;"
        "   font-size: 15px;"
        "   font-weight: 500;"
        "}"
        "QPushButton:hover {"
        "   color: #FFFFFF;"
        "   background-color: #2D2D2D;"
        "   border-radius: 10px;"
        "}"
    );
    connect(btn, &QPushButton::clicked, [this, index]() {
        emit PageChanged(index);
    });

    return btn;
};