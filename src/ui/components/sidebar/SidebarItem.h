#ifndef SIDEBARITEM_H
#define SIDEBARITEM_H

#include <QPushButton>

class SidebarItem : public QPushButton {
    Q_OBJECT
public:
    // id - унікальний ключ (наприклад, "dashboard", "wallet_1")
    SidebarItem(const QString &id, const QString &text, const QString &icon, QWidget *parent = nullptr);

    QString getId() const { return m_id; }
    
    // Можна додати методи для зміни стилю "активної" кнопки
    void setActive(bool active);

private:
    QString m_id;
};

#endif // SIDEBARITEM_H