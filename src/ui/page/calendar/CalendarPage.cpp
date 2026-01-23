#include "CalendarPage.h"
#include <QTextCharFormat>

CalendarPage::CalendarPage(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void CalendarPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30); // Більші відступи для "повітря"
    mainLayout->setSpacing(20);

    // Заголовок сторінки
    QLabel *title = new QLabel("Calendar", this);
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: white; font-family: 'Segoe UI';");
    mainLayout->addWidget(title);

    // Створення календаря
    calendar = new QCalendarWidget(this);
    calendar->setGridVisible(false); // Прибираємо стандартну сітку, зробимо краще через CSS
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    calendar->setNavigationBarVisible(true);
    
    // Стилізація (CSS)
    calendar->setStyleSheet(
        // 1. Головний фон
        "QCalendarWidget QWidget { "
        "   background-color: #1E1E1E; " 
        "   color: white; "
        "}"

        // 2. Верхня панель (Навігація: Місяць Рік)
        "QCalendarWidget QWidget#qt_calendar_navigationbar { "
        "   background-color: #2D2D2D; "
        "   border-radius: 8px; "
        "   min-height: 40px; "
        "}"

        // 3. Кнопки стрілок і вибору місяця
        "QCalendarWidget QToolButton { "
        "   color: white; "
        "   background-color: transparent; "
        "   font-weight: bold; "
        "   icon-size: 20px; "
        "   border: none; "
        "   margin: 5px; "
        "}"
        "QCalendarWidget QToolButton:hover { "
        "   background-color: #383838; "
        "   border-radius: 4px; "
        "}"
        "QCalendarWidget QToolButton#qt_calendar_prevmonth { qproperty-icon: url(:/icons/left_arrow.png); }" 
        "QCalendarWidget QToolButton#qt_calendar_nextmonth { qproperty-icon: url(:/icons/right_arrow.png); }"

        // 4. Меню вибору місяця (коли клікаєш на назву)
        "QCalendarWidget QMenu { "
        "   background-color: #2D2D2D; "
        "   color: white; "
        "   border: 1px solid #444; "
        "}"
        "QCalendarWidget QMenu::item:selected { "
        "   background-color: #00E676; "
        "   color: black; "
        "}"

        // 5. Випадаючий список року (QSpinBox)
        "QCalendarWidget QSpinBox { "
        "   background-color: #2D2D2D; "
        "   color: white; "
        "   border: none; "
        "   font-size: 16px; "
        "   font-weight: bold; "
        "}"
        
        // 6. Сама таблиця днів (AbstractItemView)
        "QCalendarWidget QAbstractItemView:enabled { "
        "   font-size: 14px; "
        "   background-color: #1E1E1E; "
        "   color: #DDD; "
        "   selection-background-color: #00E676; " // Зелений акцент
        "   selection-color: black; "               // Чорний текст на зеленому
        "   outline: none; "
        "}"
        "QCalendarWidget QAbstractItemView:disabled { "
        "   color: #444; "
        "}"
    );

    // Додаткове налаштування шрифту для днів тижня (Пн, Вт...)
    QTextCharFormat format;
    format.setForeground(QColor("#888888")); // Сірий колір для назв днів
    format.setFontWeight(QFont::Bold);
    calendar->setHeaderTextFormat(format);

    mainLayout->addWidget(calendar);
}