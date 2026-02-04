#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QMap>
#include "src/ui/components/sidebar/SidebarItem.h"

class Sidebar : public QWidget {
    Q_OBJECT
public:
    explicit Sidebar(QWidget *parent = nullptr);

    // Для динамічних графіків (публічний)
    void addChartButton(const QString &id, const QString &text);

    // Оновлення тексту (для перейменування графіків)
    void updateButtonText(const QString &id, const QString &newText);

signals:
    void navigationRequested(const QString &pageId);

private:
    QVBoxLayout *mainLayout;
    QVBoxLayout *chartsLayout; // Окремий контейнер для графіків
    QMap<QString, SidebarItem*> itemsMap;

    // Допоміжні методи (ПРИВАТНІ, бо тільки Сайдбар вирішує, де стоять кнопки)
    void addHeader(const QString &text);
    void createButton(const QString &id, const QString &text, const QString &icon);
};

#endif // SIDEBAR_H