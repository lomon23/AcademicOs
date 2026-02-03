#include "FinanceFullPage.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>

FinanceFullPage::FinanceFullPage(QWidget *parent) : QWidget(parent) {
    // Головний лейаут сторінки
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 1. HEADER (Як в Analytics)
    QWidget *headerContainer = new QWidget();
    headerContainer->setFixedHeight(60);
    headerContainer->setStyleSheet("background-color: #1E1E1E; border-bottom: 1px solid #333;");
    
    QHBoxLayout *headerLayout = new QHBoxLayout(headerContainer);
    headerLayout->setContentsMargins(20, 0, 20, 0);
    
    QLabel *title = new QLabel("Wallet");
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    headerLayout->addWidget(title);
    
    mainLayout->addWidget(headerContainer);

    // 2. BODY (Sidebar + Workspace)
    QHBoxLayout *bodyLayout = new QHBoxLayout();
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(0);

    sidebar = new FinanceSidebar(this);
    workspace = new FinanceWorkspace(this);

    bodyLayout->addWidget(sidebar);
    bodyLayout->addWidget(workspace, 1); // 1 = розтягується на всю ширину

    mainLayout->addLayout(bodyLayout);
}