#include "ChartPanel.h"

ChartPanel::ChartPanel(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    
    // Тимчасовий дизайн: Фіолетовий
    // Width не задаємо - він розтягнеться на все вільне місце!
    this->setStyleSheet("background-color: #BD93F9; border-radius: 8px;");
}