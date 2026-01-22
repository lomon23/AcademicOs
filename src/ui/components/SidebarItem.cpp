#include "SidebarItem.h"

SidebarItem::SidebarItem(const QString &id, const QString &text, const QString &icon, QWidget *parent)
    : QPushButton(parent), m_id(id)
{
    setText(icon + "  " + text);
    setFixedHeight(45);
    setCursor(Qt::PointingHandCursor);
    
    // Базовий стиль
    setStyleSheet(
        "SidebarItem { "
        "  text-align: left; "
        "  padding-left: 15px; "
        "  color: #AAAAAA; "
        "  background-color: transparent; "
        "  border: none; "
        "  border-radius: 8px; "
        "  font-size: 14px; "
        "  font-weight: 500; "
        "}"
        "SidebarItem:hover { "
        "  background-color: #2A2A2A; "
        "  color: white; "
        "}"
    );
}

void SidebarItem::setActive(bool active) {
    if (active) {
        // Стиль для вибраної кнопки (можна зробити світлішою або з акцентом)
        setStyleSheet(
            "SidebarItem { "
            "  text-align: left; "
            "  padding-left: 15px; "
            "  color: #FFFFFF; "     // Білий текст
            "  background-color: #333333; " // Світліший фон
            "  border: none; "
            "  border-radius: 8px; "
            "  font-size: 14px; "
            "  font-weight: 600; "
            "}"
        );
    } else {
        // Повертаємо звичайний стиль (копія з конструктора)
        setStyleSheet(
            "SidebarItem { "
            "  text-align: left; "
            "  padding-left: 15px; "
            "  color: #AAAAAA; "
            "  background-color: transparent; "
            "  border: none; "
            "  border-radius: 8px; "
            "  font-size: 14px; "
            "  font-weight: 500; "
            "}"
            "SidebarItem:hover { "
            "  background-color: #2A2A2A; "
            "  color: white; "
            "}"
        );
    }
}