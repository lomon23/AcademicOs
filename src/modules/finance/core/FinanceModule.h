#ifndef FINANCEMODULE_H
#define FINANCEMODULE_H

#include <QObject>
#include <QList>
#include <QDate>
#include <QMap>
#include <QJsonObject>

// --- СТРУКТУРИ ДАНИХ ---

struct Transaction {
    QString id;          // Унікальний ID (наприклад, timestamp)
    QDate date;          // Дата транзакції
    double amount;       // Сума (мінус = витрата, плюс = дохід)
    QString category;    // "Food", "Transport"
    QString note;        // "Кава", "Зарплата"
    QString accountName; // З якого рахунку списано (напр. "Cash")
    QString description;
};

struct PlannedExpense {
    QString id;
    QDate date;
    double amount;       // Очікувана сума
    QString title;       // Назва ("Інтернет", "Оренда")
    bool isPaid;         // Чи вже оплачено
};

struct WalletAccount {
    QString name;        // "Cash", "Mono", "Stash"
    QString currency;    // "UAH", "USD"
    double balance;      // Поточний баланс
};

// --- ГОЛОВНИЙ КЛАС МОДУЛЯ ---

class FinanceModule : public QObject {
    Q_OBJECT

public:
    static FinanceModule& instance();

    // --- Основні операції ---
    void addAccount(const QString &name, const QString &currency, double initialBalance);
    void addTransaction(const QDate &date, double amount, const QString &category, const QString &note, const QString &accountName);
    
    // --- Планування ---
    void addPlannedExpense(const QDate &date, double amount, const QString &title);
    void markPlannedAsPaid(const QString &id); // Перетворює план на транзакцію

    // --- Гетери (Отримання даних) ---
    QList<WalletAccount> getAccounts() const;
    QList<Transaction> getTransactions() const;
    QList<PlannedExpense> getPlannedExpenses() const;
    
    // --- Аналітика Балансу ---
    double getTotalBalanceInUAH() const;   // Загальний баланс (конвертований)
    double getDisposableBalance() const;   // Баланс мінус плановані витрати на цей місяць

    // --- Налаштування ---
    void setExchangeRate(const QString &currency, double rate); // USD -> 42.0

signals:
    void dataChanged(); // Сигнал для UI: "Перемалюйся!"

private:
    FinanceModule();
    ~FinanceModule();

    // Збереження/Завантаження
    void loadData();
    void saveData();
    QString getFilePath() const;

    QList<WalletAccount> accounts;
    QList<Transaction> transactions;
    QList<PlannedExpense> planned;
    
    QMap<QString, double> exchangeRates; // Кеш курсів валют
};

#endif // FINANCEMODULE_H