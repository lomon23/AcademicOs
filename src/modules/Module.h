#ifndef MODULE_H
#define MODULE_H

#include <QObject>
#include <QString>

// Базовий клас для ВСІХ модулів
class Module : public QObject {
    Q_OBJECT
public:
    explicit Module(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~Module() {}

    // Чисто віртуальні методи (кожен модуль МУСИТЬ їх мати)
    virtual void save() = 0;
    virtual void load() = 0;

    // Спільні методи для назви
    void setTitle(const QString &t) { title = t; }
    QString getTitle() const { return title; }

protected:
    QString title; // Спільна змінна для всіх
};

#endif // MODULE_H