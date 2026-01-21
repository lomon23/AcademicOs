#include "FinanceModule.h"
#include "FinanceFullPage.h"
#include "FinanceSmallWidget.h"

FinanceModule::FinanceModule(QObject *parent) : QObject(parent)
{
    moduleTitle = "Wallet";
    totalBalance = 0.0;
    
    // Завантажуємо дані при старті
    loadData();
}

void FinanceModule::setTotalBalance(double amount) {
    totalBalance = amount;
    saveData();
    //emit dataChanged();
}

void FinanceModule::addAllocation(const QString &name, double amount, AllocationType type) {
    allocations.append(Allocation(name, amount, type));
    saveData();
    emit dataChanged();
}

void FinanceModule::removeAllocation(int index) {
    if (index >= 0 && index < allocations.size()) {
        allocations.removeAt(index);
        saveData();
        emit dataChanged();
    }
}

void FinanceModule::updateAllocation(int index, const Allocation &alloc) {
    if (index >= 0 && index < allocations.size()) {
        allocations[index] = alloc;
        saveData();
        // emit dataChanged(); // Можна не викликати, якщо UI вже оновлено локально
    }
}

double FinanceModule::getAllocatedSum() const {
    double sum = 0.0;
    for (const auto &item : allocations) sum += item.amount;
    return sum;
}

double FinanceModule::getFreeBalance() const {
    return totalBalance - getAllocatedSum();
}

FinanceFullPage* FinanceModule::createFullPage() {
    return new FinanceFullPage(this);
}
FinanceSmallWidget* FinanceModule::createSmallWidget() {
    return new FinanceSmallWidget(this);
}

// --- JSON LOGIC ---

void FinanceModule::saveData() {
    QJsonObject rootObj;
    
    // 1. Зберігаємо баланс
    rootObj["totalBalance"] = totalBalance;

    // 2. Зберігаємо список
    QJsonArray allocArray;
    for (const auto &item : allocations) {
        QJsonObject itemObj;
        itemObj["name"] = item.name;
        itemObj["amount"] = item.amount;
        itemObj["type"] = static_cast<int>(item.type);
        allocArray.append(itemObj);
    }
    rootObj["allocations"] = allocArray;

    StorageManager::instance().saveConfig(STORAGE_KEY, rootObj);
}

void FinanceModule::loadData() {
    QVariant data = StorageManager::instance().loadConfig(STORAGE_KEY);
    
    if (data.isValid()) {
        QJsonObject rootObj = data.toJsonObject();
        
        if (rootObj.contains("totalBalance")) {
            totalBalance = rootObj["totalBalance"].toDouble();
        }

        if (rootObj.contains("allocations")) {
            allocations.clear();
            QJsonArray arr = rootObj["allocations"].toArray();
            
            for (const auto &val : arr) {
                // ВИПРАВЛЕНО ТУТ: val.toObject() замість toJsonObject()
                QJsonObject itemObj = val.toObject(); 
                
                QString name = itemObj["name"].toString();
                double amount = itemObj["amount"].toDouble();
                int typeInt = itemObj["type"].toInt();
                
                allocations.append(Allocation(name, amount, static_cast<AllocationType>(typeInt)));
            }
        }
    }
}
