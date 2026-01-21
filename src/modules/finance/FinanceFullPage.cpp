#include "FinanceFullPage.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QDebug>

FinanceFullPage::FinanceFullPage(FinanceModule *mod, QWidget *parent) 
    : QWidget(parent), module(mod)
{
    setupUi();
    updateUI(); 
    connect(module, &FinanceModule::dataChanged, this, &FinanceFullPage::updateUI);
}

void FinanceFullPage::setupUi() {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    
    // --- ЛІВА ПАНЕЛЬ ---
    QGroupBox *leftPanel = new QGroupBox(this);
    leftPanel->setStyleSheet("QGroupBox { border: 1px solid #444; border-radius: 8px; background: #181818; }");
    leftPanel->setFixedWidth(320);

    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setSpacing(15);
    leftLayout->setContentsMargins(20, 20, 20, 20);

    // 1. ЗАГОЛОВОК
    QLabel *titleLabel = new QLabel("My Wallet 💳", this);
    titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: white; border-bottom: 1px solid #333; padding-bottom: 10px;");
    leftLayout->addWidget(titleLabel);

    // 2. БАЛАНС
    leftLayout->addWidget(new QLabel("Total Balance (UAH):", this));
    
    balanceInput = new QLineEdit(this);
    balanceInput->setStyleSheet("font-size: 28px; padding: 10px; color: #69F0AE; background: #222; border: 1px solid #555; border-radius: 6px;");
    connect(balanceInput, &QLineEdit::textChanged, this, &FinanceFullPage::onBalanceChanged);
    leftLayout->addWidget(balanceInput);

    // 3. СТАТИСТИКА
    leftLayout->addWidget(new QLabel("Reserved / Debt:", this));
    allocatedLabel = new QLabel("0 ₴", this);
    allocatedLabel->setStyleSheet("font-size: 18px; color: #FFD740; font-weight: bold;");
    leftLayout->addWidget(allocatedLabel);

    leftLayout->addSpacing(10);

    leftLayout->addWidget(new QLabel("Free to Spend:", this));
    freeBalanceLabel = new QLabel("0 ₴", this);
    freeBalanceLabel->setStyleSheet("font-size: 32px; font-weight: bold;"); 
    leftLayout->addWidget(freeBalanceLabel);

    leftLayout->addStretch();
    mainLayout->addWidget(leftPanel);

    // --- ПРАВА ПАНЕЛЬ ---
    QVBoxLayout *rightLayout = new QVBoxLayout();
    
    QPushButton *btnAdd = new QPushButton("+ Add Category", this);
    btnAdd->setCursor(Qt::PointingHandCursor);
    btnAdd->setStyleSheet("QPushButton { background-color: #2C2C2C; color: white; padding: 12px; border: 1px dashed #666; border-radius: 6px; font-weight: bold; }"
                          "QPushButton:hover { background-color: #383838; border-color: #888; }");
    connect(btnAdd, &QPushButton::clicked, this, &FinanceFullPage::onAddClicked);
    rightLayout->addWidget(btnAdd);

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { background: transparent; border: none; } QScrollBar:vertical { width: 8px; background: #111; } QScrollBar::handle:vertical { background: #444; border-radius: 4px; }");
    
    listContainer = new QWidget();
    listContainer->setStyleSheet("background: transparent;");
    listLayout = new QVBoxLayout(listContainer);
    listLayout->setAlignment(Qt::AlignTop);
    listLayout->setSpacing(10);
    
    scroll->setWidget(listContainer);
    rightLayout->addWidget(scroll);

    mainLayout->addLayout(rightLayout);
}

void FinanceFullPage::updateUI() {
    if (balanceInput->text().toDouble() != module->getTotalBalance()) {
        balanceInput->setText(QString::number(module->getTotalBalance()));
    }
    updateLabelsOnly();
    renderAllocations();
}

void FinanceFullPage::updateLabelsOnly() {
    allocatedLabel->setText(QString::number(module->getAllocatedSum()) + " ₴");
    
    double free = module->getFreeBalance();
    freeBalanceLabel->setText(QString::number(free) + " ₴");
    
    if (free >= 0) freeBalanceLabel->setStyleSheet("font-size: 32px; font-weight: bold; color: #69F0AE;");
    else freeBalanceLabel->setStyleSheet("font-size: 32px; font-weight: bold; color: #FF5252;");
}

QString FinanceFullPage::getColorForType(AllocationType type) {
    switch (type) {
        case AllocationType::Debt: return "#FF5252";     
        case AllocationType::Reserved: return "#FFD740"; 
        case AllocationType::Goal: return "#00E676";     
        default: return "#FFFFFF";
    }
}

void FinanceFullPage::renderAllocations() {
    QLayoutItem *item;
    while ((item = listLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    auto &list = module->getAllocations();
    
    for (int i = 0; i < list.size(); ++i) {
        Allocation &alloc = list[i];

        QWidget *row = new QWidget();
        row->setStyleSheet("background-color: #252525; border-radius: 8px;");
        row->setFixedHeight(60);
        
        QHBoxLayout *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(15, 5, 15, 5);

        // 1. КНОПКА КОЛЬОРУ
        QPushButton *colorBtn = new QPushButton("●");
        colorBtn->setFixedSize(40, 40);
        colorBtn->setCursor(Qt::PointingHandCursor);
        QString color = getColorForType(alloc.type);
        colorBtn->setStyleSheet(QString("font-size: 24px; color: %1; background: transparent; border: none;").arg(color));
        
        connect(colorBtn, &QPushButton::clicked, [this, i, colorBtn](){
            Allocation item = module->getAllocations()[i];
            int nextType = (static_cast<int>(item.type) + 1) % 3;
            item.type = static_cast<AllocationType>(nextType);
            module->updateAllocation(i, item);
            colorBtn->setStyleSheet(QString("font-size: 24px; color: %1; background: transparent; border: none;").arg(getColorForType(item.type)));
        });
        rowLayout->addWidget(colorBtn);

        // 2. НАЗВА
        QLineEdit *nameEdit = new QLineEdit(alloc.name);
        nameEdit->setPlaceholderText("Enter name...");
        nameEdit->setStyleSheet("QLineEdit { background: #2A2A2A; border: 1px solid #333; border-radius: 4px; color: white; font-size: 16px; padding-left: 5px; } "
                                "QLineEdit:focus { border: 1px solid #666; background: #333; }");
        
        connect(nameEdit, &QLineEdit::textChanged, [this, i](const QString &text){
            Allocation item = module->getAllocations()[i];
            item.name = text;
            module->updateAllocation(i, item);
        });
        
        QPushButton *pencilBtn = new QPushButton("✏️");
        pencilBtn->setFixedSize(25, 25);
        pencilBtn->setCursor(Qt::PointingHandCursor);
        pencilBtn->setStyleSheet("background: transparent; border: none; font-size: 14px; color: #777;");
        connect(pencilBtn, &QPushButton::clicked, [nameEdit](){
            nameEdit->setFocus();
            nameEdit->selectAll();
        });

        rowLayout->addWidget(nameEdit, 1);
        rowLayout->addWidget(pencilBtn);

        // 3. СУМА
        QDoubleSpinBox *amountEdit = new QDoubleSpinBox();
        amountEdit->setMaximum(999999);
        amountEdit->setValue(alloc.amount);
        amountEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
        amountEdit->setAlignment(Qt::AlignRight);
        amountEdit->setSuffix(" ₴");
        amountEdit->setStyleSheet("QDoubleSpinBox { background: #1E1E1E; color: #EEE; border: 1px solid #444; border-radius: 4px; padding: 4px; font-weight: bold; }"
                                  "QDoubleSpinBox:focus { border: 1px solid #00E676; }");
        
        connect(amountEdit, &QDoubleSpinBox::valueChanged, [this, i](double val){
            Allocation item = module->getAllocations()[i];
            item.amount = val;
            module->updateAllocation(i, item);
            updateLabelsOnly(); 
        });
        rowLayout->addWidget(amountEdit);

        // 4. ВИДАЛИТИ
        QPushButton *btnDel = new QPushButton("×");
        btnDel->setFixedSize(30, 30);
        btnDel->setCursor(Qt::PointingHandCursor);
        btnDel->setStyleSheet("QPushButton { background: transparent; color: #555; border: none; font-size: 24px; font-weight: bold; } "
                              "QPushButton:hover { color: #FF5252; }");
        connect(btnDel, &QPushButton::clicked, [this, i](){
            module->removeAllocation(i);
        });
        rowLayout->addWidget(btnDel);

        listLayout->addWidget(row);
    }
}

// --- ОСЬ ЦІ ДВІ ФУНКЦІЇ, ЯКИХ НЕ ВИСТАЧАЛО ---
void FinanceFullPage::onBalanceChanged(const QString &text) {
    module->setTotalBalance(text.toDouble());
    updateLabelsOnly(); 
}

void FinanceFullPage::onAddClicked() {
    module->addAllocation("New Goal", 0.0, AllocationType::Goal);
}