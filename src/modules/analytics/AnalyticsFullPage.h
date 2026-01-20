#ifndef ANALYTICSFULLPAGE_H
#define ANALYTICSFULLPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include "../../../libs/qcustomplot/qcustomplot.h"

class AnalyticsFullPage : public QWidget {
    Q_OBJECT
public:
    explicit AnalyticsFullPage(QWidget *parent = nullptr);

    // Метод для повних даних (може приймати дати і значення)
    void setData(const QVector<double> &data);
    void setTitle(const QString &title);
signals:
    // Сигнал, коли ми змінили налаштування (щоб оновити малий віджет)
    void configChanged(const QString &newTitle);
private:
    QCustomPlot *chart;
    QCPTextElement *chartTitleElement;
    // Поля налаштувань
    QLineEdit *titleInput;
    QLineEdit *xAxisInput;
    QLineEdit *yAxisInput;

    void setupUi();
    void setupChart();
    
private slots:

    void onTitleChanged(const QString &text);
};
#endif // ANALYTICSFULLPAGE_H