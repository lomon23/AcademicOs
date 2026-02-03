#include "FinanceModule.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

FinanceModule& FinanceModule::instance() {
    static FinanceModule instance;
    return instance;
}

FinanceModule::FinanceModule() {
    // Дефолтний курс (можна буде міняти в налаштуваннях)
    exchangeRates["UAH"] = 1.0;
    exchangeRates["USD"] = 42.0; 
    exchangeRates["EUR"] = 45.0;

    loadData();
}

FinanceModule::~FinanceModule() {
    saveData();
}

// --- LOGIC: ACCOUNTS ---

void FinanceModule::addAccount(const QString &name, const QString &currency, double initialBalance) {
    // Перевірка на дублікати
    for (const auto &acc : accounts) {
        if (acc.name == name) return; 
    }
    accounts.append({name, currency, initialBalance});
    saveData();
    emit dataChanged();
}

// --- LOGIC: TRANSACTIONS ---

void FinanceModule::addTransaction(const QDate &date, double amount, const QString &category, const QString &note, const QString &accountName) {
    // 1. Створюємо транзакцію
    Transaction t;
    t.id = QString::number(QDateTime::currentMSecsSinceEpoch()); // Простий ID
    t.date = date;
    t.amount = amount;
    t.category = category;
    t.note = note;
    t.accountName = accountName;

    // 2. Додаємо в історію (на початок списку, щоб нові були зверху)
    transactions.prepend(t);

    // 3. Оновлюємо баланс рахунку
    for (auto &acc : accounts) {
        if (acc.name == accountName) {
            acc.balance += amount; // Amount може бути від'ємним
            break;
        }
    }

    saveData();
    emit dataChanged();
}

// --- LOGIC: PLANNED ---

void FinanceModule::addPlannedExpense(const QDate &date, double amount, const QString &title) {
    PlannedExpense p;
    p.id = QString::number(QDateTime::currentMSecsSinceEpoch());
    p.date = date;
    p.amount = amount; // Тут зазвичай сума > 0, але ми знаємо що це витрата
    p.title = title;
    p.isPaid = false;

    planned.append(p);
    saveData();
    emit dataChanged();
}

void FinanceModule::markPlannedAsPaid(const QString &id) {
    for (auto &p : planned) {
        if (p.id == id) {
            p.isPaid = true;
            // Тут можна автоматично викликати addTransaction, 
            // але краще це робити з UI, щоб уточнити суму
            break;
        }
    }
    saveData();
    emit dataChanged();
}

// --- GETTERS & MATH ---

QList<WalletAccount> FinanceModule::getAccounts() const { return accounts; }
QList<Transaction> FinanceModule::getTransactions() const { return transactions; }
QList<PlannedExpense> FinanceModule::getPlannedExpenses() const { return planned; }

double FinanceModule::getTotalBalanceInUAH() const {
    double total = 0.0;
    for (const auto &acc : accounts) {
        double rate = exchangeRates.value(acc.currency, 1.0);
        total += acc.balance * rate;
    }
    return total;
}

double FinanceModule::getDisposableBalance() const {
    double total = getTotalBalanceInUAH();
    double upcomingExpenses = 0.0;
    
    QDate today = QDate::currentDate();
    // Рахуємо плановані витрати тільки на найближчі 30 днів
    for (const auto &p : planned) {
        if (!p.isPaid && p.date >= today && p.date <= today.addDays(30)) {
            // Припускаємо що плановані витрати в грн (для MVP), 
            // або треба додати валюту і в PlannedExpense
            upcomingExpenses += p.amount; 
        }
    }
    return total - upcomingExpenses;
}

void FinanceModule::setExchangeRate(const QString &currency, double rate) {
    exchangeRates[currency] = rate;
    emit dataChanged(); // Перерахувати загальний баланс
}

// --- SAVE / LOAD SYSTEM ---

QString FinanceModule::getFilePath() const {
    // Зберігаємо поруч з analytics
    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/AcademicOS";
    QDir dir(path);
    if (!dir.exists()) dir.mkpath(".");
    return path + "/wallet_data.json";
}

void FinanceModule::saveData() {
    QJsonObject root;

    // 1. Save Accounts
    QJsonArray accArray;
    for (const auto &acc : accounts) {
        QJsonObject obj;
        obj["name"] = acc.name;
        obj["currency"] = acc.currency;
        obj["balance"] = acc.balance;
        accArray.append(obj);
    }
    root["accounts"] = accArray;

    // 2. Save Transactions
    QJsonArray transArray;
    for (const auto &t : transactions) {
        QJsonObject obj;
        obj["id"] = t.id;
        obj["date"] = t.date.toString(Qt::ISODate);
        obj["amount"] = t.amount;
        obj["category"] = t.category;
        obj["note"] = t.note;
        obj["account"] = t.accountName;
        transArray.append(obj);
    }
    root["transactions"] = transArray;

    // 3. Save Planned
    QJsonArray planArray;
    for (const auto &p : planned) {
        QJsonObject obj;
        obj["id"] = p.id;
        obj["date"] = p.date.toString(Qt::ISODate);
        obj["amount"] = p.amount;
        obj["title"] = p.title;
        obj["isPaid"] = p.isPaid;
        planArray.append(obj);
    }
    root["planned"] = planArray;

    QFile file(getFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
    }
}

void FinanceModule::loadData() {
    QFile file(getFilePath());
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();

    // 1. Load Accounts
    accounts.clear();
    QJsonArray accArray = root["accounts"].toArray();
    for (const auto &val : accArray) {
        QJsonObject obj = val.toObject();
        accounts.append({
            obj["name"].toString(),
            obj["currency"].toString(),
            obj["balance"].toDouble()
        });
    }

    // 2. Load Transactions
    transactions.clear();
    QJsonArray transArray = root["transactions"].toArray();
    for (const auto &val : transArray) {
        QJsonObject obj = val.toObject();
        Transaction t;
        t.id = obj["id"].toString();
        t.date = QDate::fromString(obj["date"].toString(), Qt::ISODate);
        t.amount = obj["amount"].toDouble();
        t.category = obj["category"].toString();
        t.note = obj["note"].toString();
        t.accountName = obj["account"].toString();
        transactions.append(t);
    }

    // 3. Load Planned
    planned.clear();
    QJsonArray planArray = root["planned"].toArray();
    for (const auto &val : planArray) {
        QJsonObject obj = val.toObject();
        PlannedExpense p;
        p.id = obj["id"].toString();
        p.date = QDate::fromString(obj["date"].toString(), Qt::ISODate);
        p.amount = obj["amount"].toDouble();
        p.title = obj["title"].toString();
        p.isPaid = obj["isPaid"].toBool();
        planned.append(p);
    }
}