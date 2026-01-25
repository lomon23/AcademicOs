#include "ToDoModule.h"
#include "../StorageManager.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QUuid>

ToDoModule::ToDoModule(QObject *parent) : Module(parent) {
    setTitle("Tasks");
    load();
}

void ToDoModule::addCategory(const QString& name, const QString& color) {
    ToDoCategory newCat;
    
    // 1. Генеруємо ID (обов'язково!)
    newCat.id = QUuid::createUuid().toString(); 
    
    // 2. Заповнюємо дані
    newCat.name = name;
    newCat.color = color;
    newCat.isExpanded = true; // Розгорнута за замовчуванням

    // 3. Додаємо в список
    categories.append(newCat);
    
    // 4. Зберігаємо
    save();
}

QString ToDoModule::addTask(const QString& title, const QString& categoryId, const QString& parentTaskId) {
    ToDoTask newTask;
    newTask.id = QUuid::createUuid().toString();
    newTask.title = title;
    newTask.categoryId = categoryId;
    newTask.parentTaskId = parentTaskId;
    newTask.isDone = false;


    // 🔥 ЛОГІКА СОРТУВАННЯ
    if (parentTaskId.isEmpty()) {
        // ВАРІАНТ 1: Це батьківська таска -> кидаємо на самий верх
        tasks.prepend(newTask);
    } 
    else {
        // ВАРІАНТ 2: Це підзадача -> шукаємо батька і вставляємо ПІД ним
        int parentIndex = -1;
        
        for (int i = 0; i < tasks.size(); ++i) {
            if (tasks[i].id == parentTaskId) {
                parentIndex = i;
                break;
            }
        }

        if (parentIndex != -1) {
            // Вставляємо одразу після батька (Parent Index + 1)
            // Це поставить нову підзадачу вище за старих дітей, 
            // але нижче самого батька. Ідеально.
            tasks.insert(parentIndex + 1, newTask);
        } else {
            // Якщо батька не знайдено (баг?), кидаємо просто вверх
            tasks.prepend(newTask);
        }
    }

    save();
    return newTask.id;
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
        obj["parentTaskId"] = t.parentTaskId;
        obj["isRecurring"] = t.isRecurring;
        obj["date"] = t.createdDate.toString(Qt::ISODate);
        
        // 🔥 ВИПРАВЛЕНО: Тепер це всередині циклу
        obj["priority"] = t.priority; 

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
        t.parentTaskId = obj["parentTaskId"].toString();
        t.isRecurring = obj["isRecurring"].toBool();
        t.createdDate = QDate::fromString(obj["date"].toString(), Qt::ISODate);
        
        // 🔥 ВИПРАВЛЕНО: Тепер це всередині циклу і ПЕРЕД додаванням у список
        t.priority = obj["priority"].toInt(0);

        if (t.isRecurring && t.createdDate != QDate::currentDate()) {
            t.isDone = false; 
            t.createdDate = QDate::currentDate();
        }
        tasks.append(t);
    }
}

QWidget* ToDoModule::createSmallWidget() { return new QWidget(); }
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

void ToDoModule::cyclePriority(const QString& taskId) {
    for (auto& task : tasks) {
        if (task.id == taskId) {
            // Математика: (0+1)%4 = 1 ... (3+1)%4 = 0
            task.priority = (task.priority + 1) % 4;
            save(); // Миттєве збереження
            return;
        }
    }
}

void ToDoModule::deleteCategory(const QString& catId) {
    // 1. Видаляємо саму категорію
    for (int i = 0; i < categories.size(); ++i) {
        if (categories[i].id == catId) {
            categories.removeAt(i);
            break;
        }
    }
    
    // 2. Видаляємо ВСІ таски цієї категорії (каскадне видалення)
    // Використовуємо removeIf (C++20 style) або класичний цикл
    auto it = tasks.begin();
    while (it != tasks.end()) {
        if (it->categoryId == catId) {
            it = tasks.erase(it);
        } else {
            ++it;
        }
    }

    save();
}

void ToDoModule::updateCategory(const QString& catId, const QString& newName, const QString& newColor) {
    for (auto& cat : categories) {
        if (cat.id == catId) {
            cat.name = newName;
            cat.color = newColor;
            save();
            return;
        }
    }
}