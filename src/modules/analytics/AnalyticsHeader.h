#ifndef ANALYTICSHEADER_H
#define ANALYTICSHEADER_H

#include <QWidget>
#include <QHBoxLayout>

class AnalyticsHeader : public QWidget {
    Q_OBJECT
public:
    explicit AnalyticsHeader(QWidget *parent = nullptr);
    void refreshTabs(); // Перемальовує вкладки при зміні

signals:
    void categorySelected(QString category); // Користувач вибрав вкладку
    void categoryAdded(); // Користувач створив нову (щоб оновити UI)
    void layoutChanged();

private slots:
    void onAddCategoryClicked(); // Слот для кнопки "+"
    void onTabContextMenu(const QPoint &pos);

private:
    QHBoxLayout *layout;
    void createTabButton(const QString &name);
};

#endif