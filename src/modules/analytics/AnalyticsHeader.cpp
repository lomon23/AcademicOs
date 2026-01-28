#include "AnalyticsHeader.h"
#include <QLabel>
#include <QFrame>

AnalyticsHeader::AnalyticsHeader(QWidget *parent) : QWidget(parent) {
    // Вмикаємо можливість стилізації фону
    setAttribute(Qt::WA_StyledBackground, true);
    
    // Тимчасовий дизайн: Червоний + фіксована висота
    this->setStyleSheet("background-color: #FF5555; border-radius: 8px;");
    this->setFixedHeight(60); 
}