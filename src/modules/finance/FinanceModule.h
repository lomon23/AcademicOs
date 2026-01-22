#ifndef FINANCEMODULE_H
#define FINANCEMODULE_H

#include "../Module.h" // Підключаємо батька
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include "../../core/StorageManager.h"

// --- Типи даних ---
enum class AllocationType { Debt, Reserved, Goal };

struct Allocation {
    QString name;
    double amount;
    AllocationType type;
    Allocation(QString n = "", double a = 0.0, AllocationType t = AllocationType::Goal) 
        : name(n), amount(a), type(t) {}
};

class FinanceFullPage;
class FinanceSmallWidget;

// Спадкуємось від Module
class FinanceModule : public Module {
    Q_OBJECT
public:
    explicit FinanceModule(QObject *parent = nullptr);

    // Специфічні методи фінансів
    double getTotalBalance() const;
    void addTransaction(const QString &category, double amount, const QString &desc);
    void setTotalBalance(double amount);
    
    // CRUD для списку
    QVector<Allocation>& getAllocations() { return allocations; }
    void addAllocation(const QString &name, double amount, AllocationType type);
    void removeAllocation(int index);
    void updateAllocation(int index, const Allocation &alloc);

    // Математика
    double getAllocatedSum() const;
    double getFreeBalance() const;

    // Фабрики віджетів
    FinanceFullPage* createFullPage();
    FinanceSmallWidget* createSmallWidget();

    // Реалізуємо методи Module (замість saveData/loadData)
    void save() override;
    void load() override;

signals:
    void dataChanged();
    void balanceUpdated(double newBalance);

private:
    // moduleTitle видалили, бо є title в батька
    double totalBalance = 0.0; // Залишили тільки одну декларацію
    QVector<Allocation> allocations;
    const QString STORAGE_KEY = "finance_wallet";
};

#endif // FINANCEMODULE_H