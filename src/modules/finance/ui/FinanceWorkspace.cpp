#include "src/modules/finance/ui/FinanceWorkspace.h"
#include "src/modules/finance/core/FinanceModule.h"

#include <QInputDialog>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QDate>
#include <QDateEdit>
#include <QToolButton>

FinanceWorkspace::FinanceWorkspace(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 20, 30, 20);
    mainLayout->setSpacing(20);

    // 1. Блок швидкого додавання
    setupQuickAdd(mainLayout);

    setupPlannedSection(mainLayout);
    // 2. Блок історії
    setupHistory(mainLayout);

    // 3. Підписка на оновлення (щоб історія оновлювалась, коли додаємо)
    connect(&FinanceModule::instance(), &FinanceModule::dataChanged, this, &FinanceWorkspace::refreshHistory);
    refreshPlannedList();
    // Завантажуємо дані при старті
    refreshHistory();
}

void FinanceWorkspace::setupQuickAdd(QVBoxLayout *parentLayout) {
    QFrame *frame = new QFrame();
    frame->setStyleSheet("background-color: #2A2A2A; border-radius: 12px;");
    QVBoxLayout *frameLayout = new QVBoxLayout(frame);
    
    QLabel *title = new QLabel("NEW TRANSACTION");
    title->setStyleSheet("color: #BD93F9; font-weight: bold; font-size: 12px; letter-spacing: 1px;");
    frameLayout->addWidget(title);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    
    // 1. Тип (Income/Expense)
    typeSelector = new QComboBox();
    typeSelector->addItems({"Expense (-)", "Income (+)"});
    typeSelector->setFixedWidth(110);
    typeSelector->setFixedHeight(35);
    typeSelector->setStyleSheet("background: #1E1E1E; color: white; border: 1px solid #444; border-radius: 6px; padding-left: 10px;");
    
    // 2. Сума
    amountInput = new QDoubleSpinBox();
    amountInput->setRange(0, 999999);
    amountInput->setButtonSymbols(QAbstractSpinBox::NoButtons);

    amountInput->setFixedWidth(100);
    amountInput->setFixedHeight(35);
    amountInput->setStyleSheet("background: #1E1E1E; color: white; border: 1px solid #444; border-radius: 6px; padding-left: 10px; font-weight: bold;");

    // 3. 🔥 РАХУНОК (Замість категорії)
    accountSelector = new QComboBox();
    accountSelector->setFixedWidth(120);
    accountSelector->setFixedHeight(35);
    accountSelector->setStyleSheet("QComboBox { background: #1E1E1E; color: white; border: 1px solid #444; border-radius: 6px; padding-left: 10px; }");
    
    // Заповнюємо рахунками одразу
    updateAccountSelector();

    // 4. Опис
    noteInput = new QLineEdit();
    noteInput->setPlaceholderText("Description");
    noteInput->setFixedHeight(35);
    noteInput->setStyleSheet("background: #1E1E1E; color: white; border: 1px solid #444; border-radius: 6px; padding-left: 10px;");

    // Кнопка Add
    QPushButton *addBtn = new QPushButton("Add");
    addBtn->setFixedWidth(80);
    addBtn->setFixedHeight(35);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet("QPushButton { background-color: #BD93F9; color: #1E1E1E; font-weight: bold; border-radius: 6px; } QPushButton:hover { background-color: #CAA6F9; }");
    
    connect(addBtn, &QPushButton::clicked, this, &FinanceWorkspace::onAddTransaction);
    connect(&FinanceModule::instance(), &FinanceModule::dataChanged, this, &FinanceWorkspace::updateAccountSelector);

    inputLayout->addWidget(typeSelector);
    inputLayout->addWidget(amountInput);
    inputLayout->addWidget(accountSelector); // <--- Тепер тут вибір рахунку
    inputLayout->addWidget(noteInput);
    inputLayout->addWidget(addBtn);

    frameLayout->addLayout(inputLayout);
    parentLayout->addWidget(frame);
}

void FinanceWorkspace::setupHistory(QVBoxLayout *parentLayout) {
    QLabel *title = new QLabel("RECENT TRANSACTIONS");
    title->setStyleSheet("color: #888; font-weight: bold; font-size: 12px; margin-top: 10px;");
    parentLayout->addWidget(title);

    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("background: transparent; border: none;");

    QWidget *content = new QWidget();
    historyLayout = new QVBoxLayout(content);
    historyLayout->setSpacing(5);
    historyLayout->setContentsMargins(0, 0, 0, 0);
    historyLayout->setAlignment(Qt::AlignTop);

    scroll->setWidget(content);
    parentLayout->addWidget(scroll);
}

void FinanceWorkspace::onAddTransaction() {
    double amount = amountInput->value();
    if (amount <= 0) return;

    bool isExpense = (typeSelector->currentIndex() == 0);
    double finalAmount = isExpense ? -amount : amount;

    // 🔥 БЕРЕМО ВИБРАНИЙ РАХУНОК
    QString selectedAccount = accountSelector->currentText();
    
    if (selectedAccount.isEmpty() || selectedAccount == "No Accounts") {
        // Захист від дурня: якщо рахунків нема, спробуємо дефолт
        selectedAccount = "Cash"; 
        FinanceModule::instance().addAccount("Cash", "UAH", 0);
    }

    QString note = noteInput->text();
    
    // Категорію поки передаємо як пусту стрічку або "General"
    QString category = isExpense ? "Expense" : "Income"; 

    FinanceModule::instance().addTransaction(
        QDate::currentDate(), 
        finalAmount, 
        category,  // <--- Категорія тепер просто заглушка
        note, 
        selectedAccount // <--- ПРАВИЛЬНИЙ РАХУНОК
    );

    // Очищаємо
    amountInput->setValue(0);
    noteInput->clear();
}
void FinanceWorkspace::refreshHistory() {
    // 1. Очищення старого контенту
    QLayoutItem *item;
    while ((item = historyLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    auto transactions = FinanceModule::instance().getTransactions();

    // 2. ПЕРЕВІРКА НА ПУСТОТУ (Empty State)
    if (transactions.isEmpty()) {
        // Створюємо контейнер, щоб відцентрувати напис
        QWidget *emptyWidget = new QWidget();
        QVBoxLayout *emptyLayout = new QVBoxLayout(emptyWidget);
        emptyLayout->setAlignment(Qt::AlignCenter);

        // Іконка або символ (можна емодзі для простоти)
        QLabel *iconLabel = new QLabel("📭"); 
        iconLabel->setStyleSheet("font-size: 48px; color: #444; background: transparent; border: none;");
        iconLabel->setAlignment(Qt::AlignCenter);

        // Текст
        QLabel *textLabel = new QLabel("No transactions yet");
        textLabel->setStyleSheet("font-size: 16px; color: #666; font-weight: bold; background: transparent; border: none;");
        textLabel->setAlignment(Qt::AlignCenter);
        
        // Підказка
        QLabel *subLabel = new QLabel("Add your first income or expense above");
        subLabel->setStyleSheet("font-size: 12px; color: #555; background: transparent; border: none;");
        subLabel->setAlignment(Qt::AlignCenter);

        emptyLayout->addWidget(iconLabel);
        emptyLayout->addWidget(textLabel);
        emptyLayout->addWidget(subLabel);

        // Додаємо цей віджет у layout історії
        historyLayout->addWidget(emptyWidget);
        
        // Додаємо stretch, щоб напис був зверху/по центру, а не розтягувався
        historyLayout->addStretch();
        return; 
    }

    // 3. ЯКЩО Є ТРАНЗАКЦІЇ (Малюємо список)
    // Йдемо з кінця (нові зверху)
    for (int i = transactions.size() - 1; i >= 0; --i) {
        const auto &t = transactions[i];

        QFrame *row = new QFrame();
        row->setStyleSheet("background-color: #252525; border-radius: 8px; margin-bottom: 2px;");
        row->setFixedHeight(50);

        QHBoxLayout *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(15, 0, 15, 0);

        // Дата
        QLabel *dateLabel = new QLabel(t.date.toString("dd MMM"));
        dateLabel->setStyleSheet("color: #666; font-size: 12px; border: none; background: transparent;");
        dateLabel->setFixedWidth(60);

        // Категорія
        QLabel *catLabel = new QLabel(t.category);
        catLabel->setStyleSheet("color: #BD93F9; font-weight: bold; font-size: 13px; border: none; background: transparent;");
        catLabel->setFixedWidth(80);

        // Опис
        QLabel *descLabel = new QLabel(t.description);
        descLabel->setStyleSheet("color: #CCC; font-size: 13px; border: none; background: transparent;");

        // Сума
        QString amountStr = QString::number(t.amount, 'f', 2) + " UAH";
        QLabel *amountLabel = new QLabel(amountStr);
        amountLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        // 1. Створюємо кнопку видалення
        QPushButton *deleteBtn = new QPushButton("×");
        deleteBtn->setFixedSize(24, 24);
        deleteBtn->setCursor(Qt::PointingHandCursor);
        deleteBtn->setStyleSheet(
            "QPushButton { color: #666666; background: transparent; border: none; font-size: 18px; font-weight: bold; }"
            "QPushButton:hover { color: #FF5555; background: #333333; border-radius: 4px; }"
        );
        if (t.amount < 0) amountLabel->setStyleSheet("color: #FF5555; font-weight: bold; border: none; background: transparent;");
        else amountLabel->setStyleSheet("color: #50FA7B; font-weight: bold; border: none; background: transparent;");
        connect(deleteBtn, &QPushButton::clicked, this, [this, i]() {
            FinanceModule::instance().removeTransaction(i);
            refreshHistory(); 
        });
        rowLayout->addWidget(dateLabel);
        rowLayout->addWidget(catLabel);
        rowLayout->addWidget(descLabel);
        rowLayout->addStretch();
        rowLayout->addWidget(amountLabel);
        rowLayout->addWidget(deleteBtn);

        historyLayout->addWidget(row);
    }
    
    // Щоб список прижимався до верху
    historyLayout->addStretch();
}
void FinanceWorkspace::setupPlannedSection(QVBoxLayout *parentLayout) {
    // --- 1. HEADER (Кнопка-дропдаун) ---
    QPushButton *headerBtn = new QPushButton("PLANNED EXPENSES ▼");
    headerBtn->setCursor(Qt::PointingHandCursor);
    headerBtn->setStyleSheet(
        "QPushButton { text-align: left; color: #BD93F9; font-weight: bold; font-size: 12px; letter-spacing: 1px; background: transparent; border: none; padding: 5px; }"
        "QPushButton:hover { color: #CAA6F9; }"
    );
    parentLayout->addWidget(headerBtn);

    // --- 2. CONTAINER (Те, що ховається) ---
    plannedContainer = new QWidget();
    QVBoxLayout *containerLayout = new QVBoxLayout(plannedContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(10);

    // Логіка згортання
    connect(headerBtn, &QPushButton::clicked, this, [this, headerBtn]() {
        bool isVisible = plannedContainer->isVisible();
        plannedContainer->setVisible(!isVisible);
        headerBtn->setText(isVisible ? "PLANNED EXPENSES ▶" : "PLANNED EXPENSES ▼");
    });

    // --- 3. INPUT FORM ---
    QFrame *inputFrame = new QFrame();
    inputFrame->setStyleSheet("background-color: #2A2A2A; border-radius: 12px;");
    QHBoxLayout *inputLayout = new QHBoxLayout(inputFrame);

    // Дата
    planDateInput = new QDateEdit(QDate::currentDate().addDays(1)); // Завтра
    planDateInput->setCalendarPopup(true);
    planDateInput->setDisplayFormat("dd.MM");
    planDateInput->setFixedWidth(80);
    planDateInput->setFixedHeight(35);
    planDateInput->setStyleSheet("background: #1E1E1E; color: white; border: 1px solid #444; border-radius: 6px;");

    // Сума
    planAmountInput = new QDoubleSpinBox();
    planAmountInput->setRange(0, 999999);
    planAmountInput->setButtonSymbols(QAbstractSpinBox::NoButtons);
    planAmountInput->setFixedWidth(90);
    planAmountInput->setFixedHeight(35);
    planAmountInput->setStyleSheet("background: #1E1E1E; color: white; border: 1px solid #444; border-radius: 6px; padding-left: 5px;");

    // Назва
    planTitleInput = new QLineEdit();
    planTitleInput->setPlaceholderText("Title (e.g. Internet)");
    planTitleInput->setFixedHeight(35);
    planTitleInput->setStyleSheet("background: #1E1E1E; color: white; border: 1px solid #444; border-radius: 6px; padding-left: 5px;");

    // Кнопка Add
    QPushButton *addBtn = new QPushButton("Plan");
    addBtn->setFixedWidth(60);
    addBtn->setFixedHeight(35);
    addBtn->setStyleSheet("background-color: #44475a; color: white; border-radius: 6px;");
    
    // Логіка додавання
    connect(addBtn, &QPushButton::clicked, this, [this](){
        double amt = planAmountInput->value();
        if (amt <= 0) return;
        
        FinanceModule::instance().addPlannedExpense(
            planDateInput->date(),
            amt,
            planTitleInput->text()
        );
        
        // Очистка полів
        planAmountInput->setValue(0);
        planTitleInput->clear();

        // 🔥 ФІКС: Примусово оновлюємо список ПРЯМО ЗАРАЗ
        refreshPlannedList(); 
    });

    inputLayout->addWidget(planDateInput);
    inputLayout->addWidget(planAmountInput);
    inputLayout->addWidget(planTitleInput);
    inputLayout->addWidget(addBtn);
    
    containerLayout->addWidget(inputFrame);

    // --- 4. LIST AREA ---
    QWidget *listWidget = new QWidget();
    plannedListLayout = new QVBoxLayout(listWidget);
    plannedListLayout->setContentsMargins(0, 0, 0, 0);
    plannedListLayout->setSpacing(5);
    
    containerLayout->addWidget(listWidget);
    
    parentLayout->addWidget(plannedContainer);
}

void FinanceWorkspace::refreshPlannedList() {
    // 1. Чистимо старе
    QLayoutItem *item;
    while ((item = plannedListLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    // 2. Беремо дані
    auto planned = FinanceModule::instance().getPlannedExpenses();
    
    // Якщо пусто — нічого не робимо
    if (planned.isEmpty()) return;

    for (const auto &p : planned) {
        if (p.isPaid) continue; 

        QFrame *row = new QFrame();
        row->setStyleSheet("background-color: #252525; border-radius: 6px; border-left: 3px solid #BD93F9;");
        row->setFixedHeight(45);
        
        QHBoxLayout *layout = new QHBoxLayout(row);
        layout->setContentsMargins(10, 0, 10, 0);
        
        QLabel *dateLbl = new QLabel(p.date.toString("dd.MM"));
        dateLbl->setStyleSheet("color: #888; border: none; font-size: 11px;");
        
        QLabel *titleLbl = new QLabel(p.title);
        titleLbl->setStyleSheet("color: white; font-weight: bold; border: none; font-size: 13px; margin-left: 5px;");
        
        QLabel *amountLbl = new QLabel("-" + QString::number(p.amount, 'f', 0) + " UAH");
        amountLbl->setStyleSheet("color: #FF79C6; font-weight: bold; border: none;");
        
        QPushButton *payBtn = new QPushButton("Pay");
        payBtn->setFixedSize(50, 24);
        payBtn->setCursor(Qt::PointingHandCursor);
        payBtn->setStyleSheet("background-color: #50FA7B; color: #1E1E1E; border-radius: 4px; font-weight: bold; border: none;");
        
        // --- 🔥 ЛОГІКА З ВИБОРОМ РАХУНКУ ---
        connect(payBtn, &QPushButton::clicked, this, [this, p](){
            // 1. Отримуємо список всіх рахунків
            auto accounts = FinanceModule::instance().getAccounts();
            if (accounts.isEmpty()) return;

            QStringList items;
            for (const auto &acc : accounts) {
                items << acc.name; // "mono", "cash", "stash"
            }

            // 2. Показуємо діалогове вікно вибору
            bool ok;
            QString selectedAccount = QInputDialog::getItem(
                this, 
                "Pay Planned Expense", // Заголовок
                "Choose Account:",     // Текст
                items,                 // Варіанти
                0,                     // Дефолтний вибір (перший)
                false,                 // Заборона редагування тексту
                &ok
            );

            // 3. Якщо натиснули OK і вибрали рахунок
            if (ok && !selectedAccount.isEmpty()) {
                FinanceModule::instance().addTransaction(
                    QDate::currentDate(), 
                    -p.amount, 
                    "Planned", 
                    p.title, 
                    selectedAccount // <--- Платимо з вибраного!
                );
                
                FinanceModule::instance().markPlannedAsPaid(p.id);
                refreshPlannedList(); 
            }
        });
        // -----------------------------------

        layout->addWidget(dateLbl);
        layout->addWidget(titleLbl, 1);
        layout->addWidget(amountLbl);
        layout->addWidget(payBtn);
        
        plannedListLayout->addWidget(row);
        row->show();
    }
    
    // ВАЖЛИВО: Оновити відображення батьківського віджета
    plannedContainer->update(); 
}
void FinanceWorkspace::updateAccountSelector() {
    accountSelector->clear();
    
    auto accounts = FinanceModule::instance().getAccounts();
    if (accounts.isEmpty()) {
        accountSelector->addItem("No Accounts");
        return;
    }

    for (const auto &acc : accounts) {
        accountSelector->addItem(acc.name);
    }
}