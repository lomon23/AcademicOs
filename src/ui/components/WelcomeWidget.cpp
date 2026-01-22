#include "WelcomeWidget.h"

WelcomeWidget::WelcomeWidget(QWidget *parent) : QWidget(parent)
{
    // 1. Налаштування розмірів
    this->setFixedHeight(180); // Трохи компактніше, ніж 200, але можна змінити
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // Розтягується в ширину

    // 2. Стилізація (CSS)
    // Градієнт від темно-сірого до трохи світлішого виглядає "преміально"
    this->setStyleSheet(
        "WelcomeWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #252525, stop:1 #2E2E2E);"
        "   border-radius: 20px;"
        "   border: 1px solid #3A3A3A;"
        "}"
        "QLabel#Greeting {"
        "   color: white;"
        "   font-size: 32px;"
        "   font-weight: bold;"
        "}"
        "QLabel#Date {"
        "   color: #AAAAAA;"
        "   font-size: 16px;"
        "}"
        "QPushButton {"
        "   background-color: #00E676;" /* Твій зелений акцент */
        "   color: #121212;"
        "   border-radius: 10px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   padding: 10px 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #00C853;"
        "}"
    );

    // 3. Лейаути
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(40, 20, 40, 20); // Відступи всередині картки

    // ЛІВА ЧАСТИНА (Тексти)
    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->setSpacing(5);
    
    greetingLabel = new QLabel(this);
    greetingLabel->setObjectName("Greeting"); // Для CSS
    
    dateLabel = new QLabel(this);
    dateLabel->setObjectName("Date");     // Для CSS

    textLayout->addStretch();
    textLayout->addWidget(greetingLabel);
    textLayout->addWidget(dateLabel);
    textLayout->addStretch();

    // ПРАВА ЧАСТИНА (Кнопка)
    QVBoxLayout *btnLayout = new QVBoxLayout();
    startBtn = new QPushButton("Start Day Flow ->", this);
    startBtn->setCursor(Qt::PointingHandCursor);
    startBtn->setFixedWidth(160); // Фіксована ширина кнопки

    connect(startBtn, &QPushButton::clicked, this, &WelcomeWidget::startDayClicked);

    btnLayout->addStretch();
    btnLayout->addWidget(startBtn);
    btnLayout->addStretch();

    // Збираємо все разом
    mainLayout->addLayout(textLayout);
    mainLayout->addStretch(); // Пружина між текстом і кнопкою
    mainLayout->addLayout(btnLayout);

    // 4. Таймер
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &WelcomeWidget::updateTime);
    timer->start(60000); // Оновлюємо раз на хвилину
    
    updateTime(); // Перший запуск одразу
}

QString WelcomeWidget::getGreeting() const {
    int hour = QTime::currentTime().hour();
    
    // Хардкод імені, як і домовлялись
    QString name = "Lomon"; 

    if (hour >= 5 && hour < 12) return "Good Morning, " + name;
    if (hour >= 12 && hour < 18) return "Good Afternoon, " + name;
    if (hour >= 18 && hour < 23) return "Good Evening, " + name;
    return "Good Night, " + name;
}

void WelcomeWidget::updateTime() {
    greetingLabel->setText(getGreeting());
    // Формат дати: "Wednesday, January 22"
    dateLabel->setText(QDate::currentDate().toString("dddd, MMMM d"));
}