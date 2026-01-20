#include "AnalyticsSmallWidget.h"

#include <QMouseEvent> // Не забудь!

AnalyticsSmallWidget::AnalyticsSmallWidget(QWidget *parent) : QWidget(parent)
{
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setFixedSize(300, 200);
    
    // Курсор "Рука", щоб було видно, що можна клікнути
    this->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(15, 15, 15, 10);

    titleLabel = new QLabel("New Chart", this); // Дефолтна назва
    titleLabel->setStyleSheet("color: #AAAAAA; font-weight: bold; font-size: 14px;");
    layout->addWidget(titleLabel);

    chart = new QCustomPlot(this);
    layout->addWidget(chart);
    setupChart();
}

void AnalyticsSmallWidget::setTitle(const QString &title) {
    titleLabel->setText(title);
}

void AnalyticsSmallWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}

void AnalyticsSmallWidget::setupChart()
{
    // 1. Стиль: Прибираємо ВСЕ зайве (осі, сітку, рамку)
    chart->xAxis->setVisible(false);
    chart->yAxis->setVisible(false);
    chart->axisRect()->setAutoMargins(QCP::msNone); // Щоб не було білих полів
    chart->axisRect()->setMargins(QMargins(0, 5, 0, 5)); // Мікро-відступ зверху/знизу
    
    // Прозорий фон, щоб зливався з карткою
    chart->setBackground(Qt::NoBrush);
    chart->axisRect()->setBackground(Qt::NoBrush);

    // 2. Додаємо лінію
    chart->addGraph();
    // Яскраво-зелений колір (Accent Color)
    chart->graph(0)->setPen(QPen(QColor("#00E676"), 3)); 
    
    // Градієнт під лінією (красивий ефект)
    QColor color(0, 230, 118);
    color.setAlpha(50); // Напівпрозорий
    chart->graph(0)->setBrush(QBrush(color));
}

void AnalyticsSmallWidget::setData(const QVector<double> &data)
{
    QVector<double> x(data.size());
    for (int i = 0; i < data.size(); ++i) {
        x[i] = i; // Просто індекси по X
    }

    chart->graph(0)->setData(x, data);
    chart->rescaleAxes(); // Авто-масштаб
    chart->replot();
}