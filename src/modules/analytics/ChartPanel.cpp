#include "ChartPanel.h"
#include <QVBoxLayout>
#include <QDateTime>
#include "../../core/analytics/AnalyticsService.h"

ChartPanel::ChartPanel(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    customPlot = new QCustomPlot();
    setupDarkTheme();
    
    layout->addWidget(customPlot);
    updateChart(); // Малюємо пустий графік на старті
}

void ChartPanel::setupDarkTheme() {
    // Темний фон
    customPlot->setBackground(QBrush(QColor("#1E1E1E")));
    
    // Осі
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

    // Сітка (Grid) - ледь помітна
    customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 0, Qt::DotLine));
    customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 0, Qt::DotLine));
    customPlot->xAxis->grid()->setSubGridVisible(false);
    customPlot->yAxis->grid()->setSubGridVisible(false);

    // Формат дати
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("MMM dd");
    customPlot->xAxis->setTicker(dateTicker);
}

void ChartPanel::updateChart(const QStringList &metricIds) {
    customPlot->clearGraphs(); 
    
    if (metricIds.isEmpty()) {
        customPlot->yAxis->setLabel("Select metrics...");
        customPlot->replot();
        return;
    }

    auto metrics = AnalyticsService::instance().getAllMetrics();
    double maxVal = 0;

    for (int i = 0; i < metricIds.size(); ++i) {
        QString currentId = metricIds[i];

        const Metric *target = nullptr;
        for (const auto &m : metrics) {
            if (m.id == currentId) {
                target = &m;
                break;
            }
        }
        if (!target) continue;

        QVector<double> x, y;
        for (auto it = target->history.begin(); it != target->history.end(); ++it) {
            QDate date = QDate::fromString(it.key(), "yyyy-MM-dd");
            QDateTime dt(date, QTime(0, 0, 0));
            
            double val = it.value();
            x.append(dt.toMSecsSinceEpoch() / 1000.0);
            y.append(val);
            
            if (val > maxVal) maxVal = val;
        }

        customPlot->addGraph();
        customPlot->graph(i)->setData(x, y);
        customPlot->graph(i)->setName(target->name);

        // --- ВИПРАВЛЕННЯ ТУТ ---
        QColor graphColor;
        if (QColor::isValidColorName(target->color)) { // Було isValidColor
            graphColor = QColor(target->color);
        } else {
            graphColor = NeonPalette::getColor(i);
        }
        // -----------------------
        
        QPen pen;
        pen.setColor(graphColor);
        pen.setWidth(3);
        customPlot->graph(i)->setPen(pen);

        QLinearGradient gradient(0, 0, 0, 400);
        gradient.setColorAt(0, QColor(graphColor.red(), graphColor.green(), graphColor.blue(), 60)); 
        gradient.setColorAt(1, QColor(graphColor.red(), graphColor.green(), graphColor.blue(), 0));   
        customPlot->graph(i)->setBrush(QBrush(gradient));
    }

    customPlot->rescaleAxes();
    if (maxVal > 0) customPlot->yAxis->setRangeUpper(maxVal * 1.2);
    
    customPlot->legend->setVisible(metricIds.size() > 0);
    customPlot->legend->setBrush(QBrush(QColor(30, 30, 30, 200)));
    customPlot->legend->setBorderPen(Qt::NoPen);
    customPlot->legend->setTextColor(Qt::white);

    customPlot->replot();
}