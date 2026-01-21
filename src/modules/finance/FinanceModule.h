#ifndef FINANCEMODULE_H
#define FINANCEMODULE_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include "../../core/StorageManager.h"

// --- 1. Описуємо Типи (Важливо, щоб це було ПЕРЕД класом) ---

enum class AllocationType {
    Debt,       // Червоний (Борг)
    Reserved,   // Жовтий (Зобов'язання)
    Goal        // Зелений (Ціль)
};

struct Allocation {
    QString name;
    double amount;
    AllocationType type;

    // Конструктор
    Allocation(QString n = "", double a = 0.0, AllocationType t = AllocationType::Goal) 
        : name(n), amount(a), type(t) {}
};

// Forward declaration
class FinanceFullPage;
class FinanceSmallWidget;
// --- 2. Сам Клас ---

class FinanceModule : public QObject {
    Q_OBJECT
public:
    explicit FinanceModule(QObject *parent = nullptr);

    // Робота з балансом
    void setTotalBalance(double amount);
    double getTotalBalance() const { return totalBalance; }

    // Робота зі списком (CRUD)
    QVector<Allocation>& getAllocations() { return allocations; }
    void addAllocation(const QString &name, double amount, AllocationType type);
    void removeAllocation(int index);
    void updateAllocation(int index, const Allocation &alloc);

    // Математика
    double getAllocatedSum() const;
    double getFreeBalance() const;

    // UI та інше
    FinanceFullPage* createFullPage();
    FinanceSmallWidget* createSmallWidget();
    void setTitle(const QString &title) { moduleTitle = title; }
    QString getTitle() const { return moduleTitle; }

    // Збереження
    void saveData();
    void loadData();

signals:
    void dataChanged();

private:
    QString moduleTitle;
    double totalBalance;
    QVector<Allocation> allocations;

    const QString STORAGE_KEY = "finance_wallet";
};

#endif // FINANCEMODULE_H