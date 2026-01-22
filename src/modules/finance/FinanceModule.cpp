#include "FinanceModule.h"
#include "FinanceFullPage.h"
#include "FinanceSmallWidget.h"

FinanceModule::FinanceModule(QObject *parent) : Module(parent)
{
    title = "Wallet"; // Використовуємо батьківське поле
    totalBalance = 0.0;
    load(); // Було loadData()
}

// --- Методи специфічні для фінансів ---

double FinanceModule::getTotalBalance() const {
    return totalBalance;
}

void FinanceModule::addTransaction(const QString &category, double amount, const QString &desc) {
    totalBalance += amount;
    save(); // Зберігаємо зміни
    emit balanceUpdated(totalBalance);
}

void FinanceModule::setTotalBalance(double amount) {
    totalBalance = amount;
    save(); // Було saveData()
    emit balanceUpdated(totalBalance);
}

void FinanceModule::addAllocation(const QString &name, double amount, AllocationType type) {
    allocations.append(Allocation(name, amount, type));
    save();
    emit dataChanged();
}

void FinanceModule::removeAllocation(int index) {
    if (index >= 0 && index < allocations.size()) {
        allocations.removeAt(index);
        save();
        emit dataChanged();
    }
}

void FinanceModule::updateAllocation(int index, const Allocation &alloc) {
    if (index >= 0 && index < allocations.size()) {
        allocations[index] = alloc;
        save();
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
    FinanceFullPage* page = new FinanceFullPage(this);
    
    // 👇 ДОДАЙ ЦЕЙ БЛОК (Живе оновлення)
    connect(this, &FinanceModule::balanceUpdated, page, [page](double newBal){
        page->updateUI(); 
    });
    
    return page;
}

FinanceSmallWidget* FinanceModule::createSmallWidget() {
    FinanceSmallWidget* widget = new FinanceSmallWidget(this);
    
    // 👇 ЦЕЙ БЛОК ВІДПОВІДАЄ ЗА ЖИВЕ ОНОВЛЕННЯ
    connect(this, &FinanceModule::balanceUpdated, widget, [widget](double newBal){
        widget->updateUI(); // Або метод, який оновлює цифру на лейблі
    });
    
    return widget;
}

// --- Реалізація методів Module ---

void FinanceModule::save() { // Перейменовано з saveData
    QJsonObject rootObj;
    rootObj["totalBalance"] = totalBalance;

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

void FinanceModule::load() { // Перейменовано з loadData
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
                QJsonObject itemObj = val.toObject();
                QString name = itemObj["name"].toString();
                double amount = itemObj["amount"].toDouble();
                int typeInt = itemObj["type"].toInt();
                
                allocations.append(Allocation(name, amount, static_cast<AllocationType>(typeInt)));
            }
        }
    }
}