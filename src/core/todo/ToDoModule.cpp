#include "ToDoModule.h"
#include "../StorageManager.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

#include "../../ui/page/todo/ToDoSmallWidget.h"

ToDoModule::ToDoModule(QObject *parent) : Module(parent) {
    setTitle("Tasks");
    load();
}

void ToDoModule::addCategory(const QString& name, const QString& color) {
    categories.append(ToDoCategory(name, color));
    save();
}

void ToDoModule::addTask(const QString& title, const QString& categoryId, const QString& parentTaskId) {
    tasks.append(ToDoTask(title, categoryId, parentTaskId));
    save();
}

void ToDoModule::deleteTask(const QString& taskId) {
    // Видаляємо саме завдання
    // Також треба видалити всі підзавдання (дітей)
    QVector<QString> idsToDelete;
    idsToDelete.append(taskId);

    // Шукаємо дітей (це проста реалізація, видаляє тільки 1 рівень вкладеності, 
    // але для MVP достатньо, або можна зробити рекурсію пізніше)
    for (const auto &t : tasks) {
        if (t.parentTaskId == taskId) {
            idsToDelete.append(t.id);
        }
    }

    // Remove-If ідіома
    auto it = std::remove_if(tasks.begin(), tasks.end(), [&](const ToDoTask& t){
        return idsToDelete.contains(t.id);
    });
    tasks.erase(it, tasks.end());

    save();
}

void ToDoModule::toggleTask(const QString& taskId) {
    for (auto &task : tasks) {
        if (task.id == taskId) {
            task.isDone = !task.isDone;
            save();
            break;
        }
    }
}

QVector<ToDoTask> ToDoModule::getTasksByCategory(const QString& catId) const {
    QVector<ToDoTask> filtered;
    for (const auto &task : tasks) {
        if (task.categoryId == catId) {
            filtered.append(task);
        }
    }
    return filtered;
}

// --- JSON ---

void ToDoModule::save() {
    QJsonObject root;
    QJsonArray catArray;
    for (const auto &c : categories) {
        QJsonObject obj;
        obj["id"] = c.id;
        obj["name"] = c.name;
        obj["color"] = c.color;
        obj["isExpanded"] = c.isExpanded;
        catArray.append(obj);
    }
    root["categories"] = catArray;

    QJsonArray taskArray;
    for (const auto &t : tasks) {
        QJsonObject obj;
        obj["id"] = t.id;
        obj["title"] = t.title;
        obj["isDone"] = t.isDone;
        obj["categoryId"] = t.categoryId;
        obj["parentTaskId"] = t.parentTaskId; // <--- Зберігаємо батька
        obj["isRecurring"] = t.isRecurring;
        obj["date"] = t.createdDate.toString(Qt::ISODate);
        taskArray.append(obj);
    }
    root["tasks"] = taskArray;

    StorageManager::instance().saveConfig(STORAGE_KEY, root);
}

void ToDoModule::load() {
    QVariant data = StorageManager::instance().loadConfig(STORAGE_KEY);
    if (!data.isValid()) {
        if (categories.isEmpty()) addCategory("Inbox", "#FFFFFF");
        return;
    }

    QJsonObject root = data.toJsonObject();
    
    categories.clear();
    QJsonArray catArray = root["categories"].toArray();
    for (const auto &val : catArray) {
        QJsonObject obj = val.toObject();
        ToDoCategory c;
        c.id = obj["id"].toString();
        c.name = obj["name"].toString();
        c.color = obj["color"].toString();
        c.isExpanded = obj["isExpanded"].toBool(true);
        categories.append(c);
    }

    tasks.clear();
    QJsonArray taskArray = root["tasks"].toArray();
    for (const auto &val : taskArray) {
        QJsonObject obj = val.toObject();
        ToDoTask t;
        t.id = obj["id"].toString();
        t.title = obj["title"].toString();
        t.isDone = obj["isDone"].toBool();
        t.categoryId = obj["categoryId"].toString();
        t.parentTaskId = obj["parentTaskId"].toString(); // <--- Завантажуємо батька
        t.isRecurring = obj["isRecurring"].toBool();
        t.createdDate = QDate::fromString(obj["date"].toString(), Qt::ISODate);
        
        if (t.isRecurring && t.createdDate != QDate::currentDate()) {
            t.isDone = false; 
            t.createdDate = QDate::currentDate();
        }
        tasks.append(t);
    }
}


QWidget* ToDoModule::createFullPage() { return new QWidget(); }

void ToDoModule::renameTask(const QString& taskId, const QString& newTitle) {
    for (auto &task : tasks) {
        if (task.id == taskId) {
            task.title = newTitle;
            save(); // Зберігаємо в JSON
            break;
        }
    }
}


QWidget* ToDoModule::createSmallWidget() {
    ToDoSmallWidget *w = new ToDoSmallWidget(this);
    // ВАЖЛИВО: Модуль має повідомляти віджет про зміни
    // Але у нас немає сигналу dataChanged в модулі.
    // Поки що віджет буде оновлюватись при перемальовці Dashboad.
    // Або ми можемо додати сигнал в Module.h: void dataChanged();
    return w;
}
