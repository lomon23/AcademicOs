#include "Dashboard.h"


Dashboard::Dashboard(QWidget *parent) : QWidget(parent){
    this->setAttribute(Qt::WA_StyledBackground, true);
    
    mainLayout = new QGridLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); 

    
}