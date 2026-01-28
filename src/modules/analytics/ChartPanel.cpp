#include "ChartPanel.h"
#include <QVBoxLayout>
#include <QDateTime> // <--- Обов'язково для роботи з датами
#include "../../core/analytics/AnalyticsService.h"

ChartPanel::ChartPanel(QWidget *parent) : QWidget(parent) {
    // 1. Лейаут
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 2. Створюємо графік
    customPlot = new QCustomPlot(this);
    layout->addWidget(customPlot);

    // 3. Наводимо красу
    setupDarkTheme();

    // 4. Малюємо дані при старті
    updateChart();
}

void ChartPanel::setupDarkTheme() {
    // ТУТ ТІЛЬКИ НАЛАШТУВАННЯ КОЛЬОРІВ
    
    // Фон
    customPlot->setBackground(QBrush(QColor("#1E1E1E")));
    customPlot->axisRect()->setBackground(QBrush(QColor("#1E1E1E")));

    // Осі (Білі)
    customPlot->xAxis->setBasePen(QPen(Qt::white));
    customPlot->xAxis->setTickPen(QPen(Qt::white));
    customPlot->xAxis->setSubTickPen(QPen(Qt::white));
    customPlot->xAxis->setTickLabelColor(Qt::white);
    customPlot->xAxis->setLabelColor(Qt::white);

    customPlot->yAxis->setBasePen(QPen(Qt::white));
    customPlot->yAxis->setTickPen(QPen(Qt::white));
    customPlot->yAxis->setSubTickPen(QPen(Qt::white));
    customPlot->yAxis->setTickLabelColor(Qt::white);
    customPlot->yAxis->setLabelColor(Qt::white);
    
    // Прибираємо зайві рамки
    customPlot->xAxis2->setVisible(false);
    customPlot->yAxis2->setVisible(false);

    // Формат дати
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("MMM dd"); 
    customPlot->xAxis->setTicker(dateTicker);
}

void ChartPanel::updateChart(const QString &metricId) {
    // 1. Очищаємо старі графіки
    customPlot->clearGraphs();

    // 2. Беремо всі метрики
    auto metrics = AnalyticsService::instance().getAllMetrics();
    if (metrics.empty()) return; 

    // --- ЛОГІКА ПОШУКУ (FIX) ---
    const Metric *targetMetric = nullptr;

    if (metricId.isEmpty()) {
        targetMetric = &metrics[0]; // Якщо ID не передали - беремо першу
    } else {
        // Шукаємо метрику з потрібним ID
        for (const auto &m : metrics) {
            if (m.id == metricId) {
                targetMetric = &m;
                break;
            }
        }
    }
    
    // Якщо не знайшли за ID, беремо першу (безпечний фолбек)
    if (!targetMetric) targetMetric = &metrics[0];

    // ДЕБАГ: Виводимо в консоль, що саме ми зараз малюємо
    qDebug() << "📈 Chart is drawing:" << targetMetric->name << " (ID:" << targetMetric->id << ")";

    // --- ПІДПИС ОСІ Y ---
    // Це допоможе тобі точно знати, чий графік ти бачиш
    customPlot->yAxis->setLabel(targetMetric->name); 

    // 3. Готуємо дані (використовуємо targetMetric замість metrics[0])
    QVector<double> x(targetMetric->history.size()), y(targetMetric->history.size());

    int i = 0;
    for (auto it = targetMetric->history.begin(); it != targetMetric->history.end(); ++it) {
        QString dateStr = it.key();
        double val = it.value();

        QDate date = QDate::fromString(dateStr, "yyyy-MM-dd");
        QDateTime dt(date, QTime(0, 0, 0));
        x[i] = dt.toMSecsSinceEpoch() / 1000.0; 
        y[i] = val;
        i++;
    }

    // 4. Додаємо графік
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, y);

    // Стиль (Неон)
    QPen pen;
    pen.setColor(QColor("#50FA7B"));
    pen.setWidth(3);
    customPlot->graph(0)->setPen(pen);

    // Градієнт
    QCPGraph *graph = customPlot->graph(0);
    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(80, 250, 123, 100)); 
    gradient.setColorAt(1, QColor(80, 250, 123, 0));   
    graph->setBrush(QBrush(gradient));

    // 5. Масштабуємо
    customPlot->rescaleAxes();
    customPlot->yAxis->setRangeUpper(customPlot->yAxis->range().upper * 1.2);
    
    customPlot->replot();
}