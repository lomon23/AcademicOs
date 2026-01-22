#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QMap>
#include "SidebarItem.h"

class Sidebar : public QWidget {
    Q_OBJECT
public:
    explicit Sidebar(QWidget *parent = nullptr);

    // Додає звичайну кнопку (в основний список)
    void addButton(const QString &id, const QString &text, const QString &icon);
    
    // НОВЕ: Додає кнопку в секцію "Analytics"
    void addChartButton(const QString &id, const QString &text); 

    // Оновлення тексту працює для всіх однаково
    void updateButtonText(const QString &id, const QString &newText);

signals:
    void navigationRequested(const QString &pageId);

private:
    QVBoxLayout *mainLayout;      // Головний вертикальний лейаут
    QVBoxLayout *chartsLayout;    // Лейаут СУТО для графіків
    
    QMap<QString, SidebarItem*> itemsMap;

    // Допоміжний метод для створення заголовків (наприклад "MAIN", "ANALYTICS")
    void addHeader(const QString &text);
};

#endif // SIDEBAR_H