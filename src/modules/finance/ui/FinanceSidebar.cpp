#include "src/modules/finance/ui/FinanceSidebar.h"
#include <QFrame>
#include <QPushButton>
#include <QScrollArea>

FinanceSidebar::FinanceSidebar(QWidget *parent) : QWidget(parent) {
    setFixedWidth(300); // Фіксована ширина "труби"
    setStyleSheet("background-color: #1E1E1E; border-right: 1px solid #333;");
    
    setupLayout();
    
    // Підписуємось на зміни в базі, щоб цифри оновлювались самі
    connect(&FinanceModule::instance(), &FinanceModule::dataChanged, this, &FinanceSidebar::refreshData);
    
    refreshData();
}

void FinanceSidebar::setupLayout() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // --- БЛОК 1: ЗАГАЛЬНИЙ БАЛАНС ---
    QLabel *title = new QLabel("TOTAL BALANCE");
    title->setStyleSheet("color: #888; font-size: 12px; font-weight: bold; letter-spacing: 1px; border: none;");
    
    totalBalanceLabel = new QLabel("0.00 UAH");
    totalBalanceLabel->setStyleSheet("color: white; font-size: 32px; font-weight: bold; border: none;");

    mainLayout->addWidget(title);
    mainLayout->addWidget(totalBalanceLabel);

    // --- БЛОК 2: SAFE TO SPEND (Disposable) ---
    QFrame *safeFrame = new QFrame();
    safeFrame->setStyleSheet("background-color: #2A2A2A; border-radius: 8px; border: 1px solid #444;");
    QVBoxLayout *safeLayout = new QVBoxLayout(safeFrame);
    
    QLabel *safeTitle = new QLabel("SAFE TO SPEND");
    safeTitle->setStyleSheet("color: #F1FA8C; font-size: 11px; font-weight: bold; border: none;"); // Жовтий колір
    
    disposableLabel = new QLabel("0.00 UAH");
    disposableLabel->setStyleSheet("color: #F1FA8C; font-size: 20px; font-weight: bold; border: none;");

    safeLayout->addWidget(safeTitle);
    safeLayout->addWidget(disposableLabel);
    
    mainLayout->addWidget(safeFrame);

    // --- БЛОК 3: СПИСОК РАХУНКІВ ---
    QLabel *accTitle = new QLabel("ACCOUNTS");
    accTitle->setStyleSheet("color: #888; font-size: 12px; font-weight: bold; margin-top: 10px; border: none;");
    mainLayout->addWidget(accTitle);

    // Скрол для рахунків (якщо їх буде багато)
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("background: transparent; border: none;");
    
    QWidget *scrollContent = new QWidget();
    accountsLayout = new QVBoxLayout(scrollContent);
    accountsLayout->setContentsMargins(0, 0, 0, 0);
    accountsLayout->setSpacing(10);
    accountsLayout->addStretch(); // Пружина знизу

    scroll->setWidget(scrollContent);
    mainLayout->addWidget(scroll);

    // --- КНОПКА "ADD ACCOUNT" ---
    QPushButton *addBtn = new QPushButton("+ Add Account");
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet("QPushButton { background: #333; color: white; border-radius: 6px; padding: 8px; }"
                          "QPushButton:hover { background: #444; }");
    connect(addBtn, &QPushButton::clicked, this, [this]() {
        // 1. Створюємо діалогове вікно "на льоту"
        QDialog dialog(this);
        dialog.setWindowTitle("Add New Account");
        dialog.setFixedWidth(300);
        dialog.setStyleSheet("background-color: #2A2A2A; color: white;");

        QFormLayout form(&dialog);

        // Поле Назви
        QLineEdit nameInput;
        nameInput.setPlaceholderText("e.g. Monobank");
        nameInput.setStyleSheet("background: #1E1E1E; border: 1px solid #444; padding: 5px; color: white;");
        form.addRow("Name:", &nameInput);

        // Поле Балансу
        QDoubleSpinBox balanceInput;
        balanceInput.setRange(-999999, 999999);
        balanceInput.setValue(0);
        balanceInput.setStyleSheet("background: #1E1E1E; border: 1px solid #444; padding: 5px; color: white;");
        form.addRow("Initial Balance:", &balanceInput);

        // Кнопки OK / Cancel
        QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
        form.addRow(&buttons);

        // 2. Якщо натиснули ОК
        if (dialog.exec() == QDialog::Accepted) {
            QString name = nameInput.text();
            double balance = balanceInput.value();
            
            if (!name.isEmpty()) {
                // Додаємо в базу (валюта поки хардкод UAH)
                FinanceModule::instance().addAccount(name, "UAH", balance);
                // Оновлення UI відбудеться автоматично через сигнал dataChanged
            }
        }
    });
    // (Логіку додавання поки не пишемо, просто кнопка)
    
    mainLayout->addWidget(addBtn);
}

void FinanceSidebar::refreshData() {
    auto &fin = FinanceModule::instance();

    // 1. Оновлюємо Total Balance
    double total = fin.getTotalBalanceInUAH();
    totalBalanceLabel->setText(QString::number(total, 'f', 2) + " UAH");

    // 2. Оновлюємо Disposable (Safe to Spend)
    double safe = fin.getDisposableBalance();
    
    // --- ЛОГІКА ПОПЕРЕДЖЕННЯ ---
    if (safe < 0) {
        // Якщо ми в мінусі — ЧЕРВОНИЙ і попередження
        disposableLabel->setText(QString::number(safe, 'f', 2) + " UAH");
        disposableLabel->setStyleSheet("color: #FF5555; font-size: 20px; font-weight: bold; border: none;"); // Червоний
        
        // Знаходимо лейбл заголовка (safeTitle) і міняємо текст
        // (Тобі треба було зберегти вказівник на safeTitle в хедері .h, 
        //  але якщо лінь — просто змінимо колір цифри, це теж ок).
        // Але краще змінимо колір цифри — це найбільш помітно.
    } else {
        // Якщо все ок — Жовтий/Зелений
        disposableLabel->setText(QString::number(safe, 'f', 2) + " UAH");
        disposableLabel->setStyleSheet("color: #F1FA8C; font-size: 20px; font-weight: bold; border: none;"); // Жовтий
    }

    // 3. Перебудовуємо картки (код той самий)...
    QLayoutItem *item;
    while ((item = accountsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }
    auto accounts = fin.getAccounts();
    for (const auto &acc : accounts) {
        buildAccountCard(acc);
    }
    accountsLayout->addStretch();
}

void FinanceSidebar::buildAccountCard(const WalletAccount &acc) {
    QFrame *card = new QFrame();
    // (Стиль картки залишаємо як є...)
    card->setStyleSheet("QFrame { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #44475a, stop:1 #282a36); border-radius: 10px; border: 1px solid #6272a4; }");
    card->setFixedHeight(80);

    QVBoxLayout *layout = new QVBoxLayout(card);
    
    QHBoxLayout *topRow = new QHBoxLayout();
    QLabel *name = new QLabel(acc.name);
    name->setStyleSheet("color: white; font-weight: bold; border: none; background: transparent;");
    
    QLabel *currency = new QLabel(acc.currency);
    currency->setStyleSheet("color: #bd93f9; font-weight: bold; border: none; background: transparent;");
    
    topRow->addWidget(name);
    topRow->addStretch();
    topRow->addWidget(currency);

    QLabel *balance = new QLabel(QString::number(acc.balance, 'f', 2));
    
    // 🔥 ЛОГІКА КОЛЬОРУ 🔥
    if (acc.balance < 0) {
        // Якщо мінус — червоний і жирний
        balance->setStyleSheet("color: #FF5555; font-size: 18px; font-weight: bold; border: none; background: transparent;");
    } else {
        // Якщо плюс — білий (стандарт)
        balance->setStyleSheet("color: white; font-size: 18px; border: none; background: transparent;");
    }

    layout->addLayout(topRow);
    layout->addWidget(balance);

    accountsLayout->addWidget(card);
}