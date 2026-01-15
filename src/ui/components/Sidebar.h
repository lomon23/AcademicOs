#ifndef SIDEBAR_H 
#define SIDEBAR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

class Sidebar : public QWidget {
    Q_OBJECT
public: 
    explicit Sidebar(QWidget *parent = nullptr);


signals: 
    void pageChanged(int index);

private:
    QVBoxLayout *layout;
    QPushButton* createButton(const QString &text, int index);
};

#endif 