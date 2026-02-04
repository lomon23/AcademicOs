#ifndef PAGEFACTORY_H
#define PAGEFACTORY_H

#include <QWidget>
#include <QString>

// Forward declarations (щоб не тягнути хедери сюди)
class ToDoModule;

class PageFactory {
public:
    // Ми передаємо модуль в конструктор, щоб фабрика могла роздавати його сторінкам
    explicit PageFactory(ToDoModule *todoModule);

    // Головний метод створення
    QWidget* createPage(const QString &pageId, QWidget *parent = nullptr);

private:
    ToDoModule *m_todoModule;
};

#endif // PAGEFACTORY_H