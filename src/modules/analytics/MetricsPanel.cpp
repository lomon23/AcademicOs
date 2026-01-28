#include "MetricsPanel.h"

MetricsPanel::MetricsPanel(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    
    // Тимчасовий дизайн: Зелений + фіксована ширина (як сайдбар)
    this->setStyleSheet("background-color: #50FA7B; border-radius: 8px;");
    this->setFixedWidth(300); // Ліва колонка буде вузькою
}