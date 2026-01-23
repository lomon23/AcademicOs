#include "ToDoModule.h"
#include "../StorageManager.h" // Перевір шлях до StorageManager
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

ToDoModule::ToDoModule(QObject *parent) : Module(parent) {
    // Встановлюємо назву для модуля (з Module.h)
    setTitle("Tasks");
    // Вантажимо дані при старті
    load();
}

// --- CRUD ЛОГІКА ---

void ToDoModule::addCategory(const QString& name, const QString& color) {
    categories.append(ToDoCategory(name, color));
    save(); // Миттєве збереження
}

void ToDoModule::addTask(const QString& title, const QString& categoryId) {
    tasks.append(ToDoTask(title, categoryId));
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

// --- JSON SERIALIZATION (Збереження) ---

void ToDoModule::save() {
    QJsonObject root;

    // 1. Зберігаємо категорії
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

    // 2. Зберігаємо завдання
    QJsonArray taskArray;
    for (const auto &t : tasks) {
        QJsonObject obj;
        obj["id"] = t.id;
        obj["title"] = t.title;
        obj["isDone"] = t.isDone;
        obj["categoryId"] = t.categoryId;
        obj["isRecurring"] = t.isRecurring;
        obj["date"] = t.createdDate.toString(Qt::ISODate);
        taskArray.append(obj);
    }
    root["tasks"] = taskArray;

    // 3. Відправляємо в StorageManager
    StorageManager::instance().saveConfig(STORAGE_KEY, root);
    qDebug() << "ToDoModule: Data saved.";
}

// --- JSON DESERIALIZATION (Завантаження) ---

void ToDoModule::load() {
    QVariant data = StorageManager::instance().loadConfig(STORAGE_KEY);
    
    if (!data.isValid()) {
        // Перший запуск: створюємо дефолтну категорію
        if (categories.isEmpty()) {
            addCategory("Inbox", "#FFFFFF");
        }
        return;
    }

    QJsonObject root = data.toJsonObject();
    
    // Чистимо поточні дані перед завантаженням
    categories.clear();
    tasks.clear();

    // 1. Читаємо категорії
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

    // 2. Читаємо завдання
    QJsonArray taskArray = root["tasks"].toArray();
    for (const auto &val : taskArray) {
        QJsonObject obj = val.toObject();
        ToDoTask t;
        t.id = obj["id"].toString();
        t.title = obj["title"].toString();
        t.isDone = obj["isDone"].toBool();
        t.categoryId = obj["categoryId"].toString();
        t.isRecurring = obj["isRecurring"].toBool();
        t.createdDate = QDate::fromString(obj["date"].toString(), Qt::ISODate);
        
        // Тут можна додати логіку для скидання рутини (якщо новий день)
        if (t.isRecurring && t.createdDate != QDate::currentDate()) {
            t.isDone = false; // Скидаємо
            t.createdDate = QDate::currentDate(); // Оновлюємо дату
        }

        tasks.append(t);
    }

    qDebug() << "ToDoModule: Loaded" << categories.size() << "categories and" << tasks.size() << "tasks.";
}

// --- Заглушки для UI (поки що) ---
QWidget* ToDoModule::createSmallWidget() { return new QWidget(); }
QWidget* ToDoModule::createFullPage() { return new QWidget(); }