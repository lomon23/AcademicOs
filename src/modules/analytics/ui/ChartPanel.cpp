#include "src/modules/analytics/ui/ChartPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include "src/modules/analytics/core/AnalyticsService.h"

ChartPanel::ChartPanel(QWidget *parent) : QWidget(parent) {
    // Дефолтний стан
    currentMode = Month; 
    currentDate = QDate::currentDate();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10); 

    // 1. Створюємо Toolbar
    setupToolbar(layout);

    // 2. Створюємо Графік
    customPlot = new QCustomPlot();
    customPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setupDarkTheme();
    
    layout->addWidget(customPlot);
    
    updateDateLabel();
    updateButtonsStyle();
}

void ChartPanel::setupToolbar(QVBoxLayout *layout) {
    QHBoxLayout *toolbar = new QHBoxLayout();
    toolbar->setContentsMargins(10, 10, 10, 0);

    // --- Навігація ---
    QPushButton *btnPrev = new QPushButton("<", this);
    btnPrev->setFixedSize(30, 30);
    btnPrev->setCursor(Qt::PointingHandCursor);
    btnPrev->setStyleSheet("background: #333; color: white; border-radius: 4px; font-weight: bold;");
    connect(btnPrev, &QPushButton::clicked, this, &ChartPanel::onPrevClicked);

    dateLabel = new QLabel(this);
    dateLabel->setStyleSheet("color: white; font-size: 16px; font-weight: bold; margin: 0 10px;");
    dateLabel->setAlignment(Qt::AlignCenter);
    dateLabel->setMinimumWidth(150);

    QPushButton *btnNext = new QPushButton(">", this);
    btnNext->setFixedSize(30, 30);
    btnNext->setCursor(Qt::PointingHandCursor);
    btnNext->setStyleSheet("background: #333; color: white; border-radius: 4px; font-weight: bold;");
    connect(btnNext, &QPushButton::clicked, this, &ChartPanel::onNextClicked);

    // --- Режими ---
    btnWeek = new QPushButton("Week", this);
    btnMonth = new QPushButton("Month", this);
    btnYear = new QPushButton("Year", this);

    btnWeek->setFixedSize(60, 30);
    btnMonth->setFixedSize(60, 30);
    btnYear->setFixedSize(60, 30);
    
    btnWeek->setCursor(Qt::PointingHandCursor);
    btnMonth->setCursor(Qt::PointingHandCursor);
    btnYear->setCursor(Qt::PointingHandCursor);

    connect(btnWeek, &QPushButton::clicked, [this](){ onModeChanged(Week); });
    connect(btnMonth, &QPushButton::clicked, [this](){ onModeChanged(Month); });
    connect(btnYear, &QPushButton::clicked, [this](){ onModeChanged(Year); });

    toolbar->addWidget(btnPrev);
    toolbar->addWidget(dateLabel);
    toolbar->addWidget(btnNext);
    toolbar->addStretch(); 
    toolbar->addWidget(btnWeek);
    toolbar->addWidget(btnMonth);
    toolbar->addWidget(btnYear);

    layout->addLayout(toolbar);
}

void ChartPanel::updateButtonsStyle() {
    QString baseStyle = "QPushButton { background: #2E2E2E; color: #888; border: 1px solid #444; border-radius: 4px; }"
                        "QPushButton:hover { background: #3E3E3E; color: white; }";
    QString activeStyle = "QPushButton { background: #BD93F9; color: black; border: 1px solid #BD93F9; border-radius: 4px; font-weight: bold; }";

    btnWeek->setStyleSheet(currentMode == Week ? activeStyle : baseStyle);
    btnMonth->setStyleSheet(currentMode == Month ? activeStyle : baseStyle);
    btnYear->setStyleSheet(currentMode == Year ? activeStyle : baseStyle);
}

void ChartPanel::updateDateLabel() {
    if (currentMode == Week) {
        QDate startOfWeek = currentDate.addDays(-(currentDate.dayOfWeek() - 1));
        QDate endOfWeek = startOfWeek.addDays(6);
        dateLabel->setText(startOfWeek.toString("MMM dd") + " - " + endOfWeek.toString("MMM dd, yyyy"));
    } else if (currentMode == Month) {
        dateLabel->setText(currentDate.toString("MMMM yyyy"));
    } else {
        dateLabel->setText(currentDate.toString("yyyy"));
    }
}

void ChartPanel::onPrevClicked() {
    if (currentMode == Week) currentDate = currentDate.addDays(-7);
    else if (currentMode == Month) currentDate = currentDate.addMonths(-1);
    else currentDate = currentDate.addYears(-1);
    
    updateDateLabel();
    updateChart(currentMetricIds);
}

void ChartPanel::onNextClicked() {
    if (currentMode == Week) currentDate = currentDate.addDays(7);
    else if (currentMode == Month) currentDate = currentDate.addMonths(1);
    else currentDate = currentDate.addYears(1);
    
    updateDateLabel();
    updateChart(currentMetricIds);
}

void ChartPanel::onModeChanged(int mode) {
    currentMode = static_cast<TimeMode>(mode);
    updateButtonsStyle();
    updateDateLabel();
    updateChart(currentMetricIds);
}

QPair<QDateTime, QDateTime> ChartPanel::getCurrentRange() const {
    QDateTime start, end;
    if (currentMode == Week) {
        QDate d = currentDate.addDays(-(currentDate.dayOfWeek() - 1));
        start = QDateTime(d, QTime(0, 0, 0));
        end = QDateTime(d.addDays(6), QTime(23, 59, 59));
    } else if (currentMode == Month) {
        start = QDateTime(QDate(currentDate.year(), currentDate.month(), 1), QTime(0, 0, 0));
        end = QDateTime(QDate(currentDate.year(), currentDate.month(), currentDate.daysInMonth()), QTime(23, 59, 59));
    } else { 
        start = QDateTime(QDate(currentDate.year(), 1, 1), QTime(0, 0, 0));
        end = QDateTime(QDate(currentDate.year(), 12, 31), QTime(23, 59, 59));
    }
    return {start, end};
}

// === ГОЛОВНИЙ МЕТОД ОНОВЛЕННЯ ===
void ChartPanel::updateChart(const QStringList &metricIds) {
    if (!metricIds.isEmpty()) currentMetricIds = metricIds;

    customPlot->clearGraphs();
    
    if (currentMetricIds.isEmpty()) {
        customPlot->yAxis->setLabel("Select metrics...");
        customPlot->replot();
        return;
    }
    customPlot->yAxis->setLabel(""); 

    // 1. Осі X
    auto range = getCurrentRange();
    double start = range.first.toMSecsSinceEpoch() / 1000.0;
    double end = range.second.toMSecsSinceEpoch() / 1000.0;
    setupXAxis(start, end);

    // 2. Дані
    auto allMetrics = AnalyticsService::instance().getAllMetrics();
    double globalMaxVal = 0;
    int graphIndex = 0;

    for (const QString &id : currentMetricIds) {
        auto it = std::find_if(allMetrics.begin(), allMetrics.end(), 
                              [&id](const Metric& m){ return m.id == id; });
        
        if (it == allMetrics.end()) continue;
        const Metric &metric = *it;

        QVector<double> x, y;
        for (auto hIt = metric.history.begin(); hIt != metric.history.end(); ++hIt) {
            QDate d = QDate::fromString(hIt.key(), "yyyy-MM-dd");
            QDateTime dt(d, QTime(12, 0)); 
            
            if (dt >= range.first.addDays(-1) && dt <= range.second.addDays(1)) {
                double val = hIt.value();
                x.append(dt.toMSecsSinceEpoch() / 1000.0);
                y.append(val);
                if (val > globalMaxVal) globalMaxVal = val;
            }
        }

        customPlot->addGraph();
        customPlot->graph(graphIndex)->setData(x, y);
        customPlot->graph(graphIndex)->setName(metric.name);
        
        applyGraphStyle(customPlot->graph(graphIndex), metric, graphIndex);
        graphIndex++;
    }

    // 3. Осі Y
    setupYAxisScaling(globalMaxVal);
    
    customPlot->legend->setVisible(true);
    customPlot->replot();
}

// === ХЕЛПЕРИ ===

void ChartPanel::setupXAxis(double start, double end) {
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    
    if (currentMode == Week) {
        dateTicker->setDateTimeFormat("ddd\ndd"); 
        dateTicker->setTickCount(7);
    } else if (currentMode == Month) {
        dateTicker->setDateTimeFormat("d"); 
        dateTicker->setTickCount(10); 
    } else {
        dateTicker->setDateTimeFormat("MMM"); 
        dateTicker->setTickCount(12);
    }
    
    customPlot->xAxis->setTicker(dateTicker);
    customPlot->xAxis->setRange(start, end);
}

void ChartPanel::applyGraphStyle(QCPGraph *graph, const Metric &metric, int index) {
    QColor color;
    if (QColor::isValidColorName(metric.color)) {
        color = QColor(metric.color);
    } else {
        color = NeonPalette::getColor(index);
    }

    QPen pen;
    pen.setColor(color);
    pen.setWidth(3);
    graph->setPen(pen);

    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(color.red(), color.green(), color.blue(), 40)); 
    gradient.setColorAt(1, QColor(color.red(), color.green(), color.blue(), 0));   
    graph->setBrush(QBrush(gradient));

    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 
                           QPen(Qt::white, 1.5), QBrush(color), 6));
}

void ChartPanel::setupYAxisScaling(double globalMax) {
    if (currentMetricIds.size() == 1) {
        auto metrics = AnalyticsService::instance().getAllMetrics();
        auto it = std::find_if(metrics.begin(), metrics.end(), 
                              [this](const Metric& m){ return m.id == currentMetricIds.first(); });
        
        if (it != metrics.end()) {
            const Metric &m = *it;
            // Якщо користувач задав конкретні межі (наприклад 0-10)
            if (m.maxVal > m.minVal) {
                customPlot->yAxis->setRange(m.minVal, m.maxVal);
                return; // Виходимо, масштаб зафіксовано
            }
        }
    }

    // 2. Авто-масштаб для Y (якщо метрик багато або межі не задані)
    if (globalMax > 0) {
        // Ставимо від 0 до Максимуму + 20% "повітря" зверху
        customPlot->yAxis->setRange(0, globalMax * 1.2); 
    } else {
        // Якщо даних немає взагалі (пусто), ставимо дефолтну шкалу 0-10
        customPlot->yAxis->setRange(0, 10); 
    }
}

void ChartPanel::setupDarkTheme() {
    customPlot->setBackground(QBrush(QColor("#1E1E1E")));
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

    customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 0, Qt::DotLine));
    customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 0, Qt::DotLine));
    
    customPlot->legend->setBrush(QBrush(QColor(30, 30, 30, 200)));
    customPlot->legend->setBorderPen(Qt::NoPen);
    customPlot->legend->setTextColor(Qt::white);
}