#include "ChartPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include "../../core/analytics/AnalyticsService.h"

ChartPanel::ChartPanel(QWidget *parent) : QWidget(parent) {
    // Дефолтний стан
    currentMode = Month; 
    currentDate = QDate::currentDate();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10); // Відступ між тулбаром і графіком

    // 1. Створюємо Toolbar
    setupToolbar(layout);

    // 2. Створюємо Графік
    customPlot = new QCustomPlot();
    customPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setupDarkTheme();
    
    layout->addWidget(customPlot);
    
    // Оновлюємо UI
    updateDateLabel();
    updateButtonsStyle();
}

void ChartPanel::setupToolbar(QVBoxLayout *layout) {
    QHBoxLayout *toolbar = new QHBoxLayout();
    toolbar->setContentsMargins(10, 10, 10, 0);

    // --- ЛІВА ЧАСТИНА: Навігація (< Дата >) ---
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

    // --- ПРАВА ЧАСТИНА: Режими (Week | Month | Year) ---
    btnWeek = new QPushButton("Week", this);
    btnMonth = new QPushButton("Month", this);
    btnYear = new QPushButton("Year", this);

    // Спільний стиль для кнопок режимів буде в updateButtonsStyle()
    btnWeek->setCursor(Qt::PointingHandCursor);
    btnMonth->setCursor(Qt::PointingHandCursor);
    btnYear->setCursor(Qt::PointingHandCursor);
    
    btnWeek->setFixedSize(60, 30);
    btnMonth->setFixedSize(60, 30);
    btnYear->setFixedSize(60, 30);

    connect(btnWeek, &QPushButton::clicked, [this](){ onModeChanged(Week); });
    connect(btnMonth, &QPushButton::clicked, [this](){ onModeChanged(Month); });
    connect(btnYear, &QPushButton::clicked, [this](){ onModeChanged(Year); });

    // Збираємо все до купи
    toolbar->addWidget(btnPrev);
    toolbar->addWidget(dateLabel);
    toolbar->addWidget(btnNext);
    toolbar->addStretch(); // Пружина посередині
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
        // Показуємо: "Feb 02 - Feb 08, 2026"
        QDate startOfWeek = currentDate.addDays(-(currentDate.dayOfWeek() - 1));
        QDate endOfWeek = startOfWeek.addDays(6);
        dateLabel->setText(startOfWeek.toString("MMM dd") + " - " + endOfWeek.toString("MMM dd, yyyy"));
    } else if (currentMode == Month) {
        // Показуємо: "February 2026"
        dateLabel->setText(currentDate.toString("MMMM yyyy"));
    } else {
        // Показуємо: "2026"
        dateLabel->setText(currentDate.toString("yyyy"));
    }
}

void ChartPanel::onPrevClicked() {
    if (currentMode == Week) currentDate = currentDate.addDays(-7);
    else if (currentMode == Month) currentDate = currentDate.addMonths(-1);
    else currentDate = currentDate.addYears(-1);
    
    updateDateLabel();
    updateChart(currentMetricIds); // Перемальовуємо з новими датами
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
    // При зміні режиму скидаємо дату на "сьогодні", або залишаємо як є - як зручніше.
    // Поки залишаємо поточну дату як якір.
    updateButtonsStyle();
    updateDateLabel();
    updateChart(currentMetricIds);
}

// === ЛОГІКА ГРАФІКА ===

void ChartPanel::updateChart(const QStringList &metricIds) {
    // Якщо передали пустий список, але у нас є збережений - використовуємо його
    // Якщо передали новий список - оновлюємо збережений
    if (!metricIds.isEmpty()) {
        currentMetricIds = metricIds;
    }

    customPlot->clearGraphs();
    
    if (currentMetricIds.isEmpty()) {
        customPlot->yAxis->setLabel("Select metrics...");
        customPlot->replot();
        return;
    }

    // 1. Розраховуємо межі часу (X Axis Range)
    auto range = getCurrentRange();
    double start = range.first.toMSecsSinceEpoch() / 1000.0;
    double end = range.second.toMSecsSinceEpoch() / 1000.0;

    // 2. Налаштовуємо вісь X під режим
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    
    if (currentMode == Week) {
        dateTicker->setDateTimeFormat("ddd\ndd"); // Mon 02
        dateTicker->setTickCount(7); // Хочемо бачити кожен день
    } else if (currentMode == Month) {
        dateTicker->setDateTimeFormat("d"); // 1, 5, 10...
        dateTicker->setTickCount(10); // Не ліпимо 30 чисел, буде каша. Qt саме підбере крок.
    } else {
        dateTicker->setDateTimeFormat("MMM"); // Jan, Feb
        dateTicker->setTickCount(12);
    }
    customPlot->xAxis->setTicker(dateTicker);
    customPlot->xAxis->setRange(start, end); // Жорстко ставимо межі

    // 3. Малюємо дані
    auto metrics = AnalyticsService::instance().getAllMetrics();
    double maxVal = 0;

    for (int i = 0; i < currentMetricIds.size(); ++i) {
        QString currentId = currentMetricIds[i];
        const Metric *target = nullptr;
        for (const auto &m : metrics) {
            if (m.id == currentId) { target = &m; break; }
        }
        if (!target) continue;

        QVector<double> x, y;
        for (auto it = target->history.begin(); it != target->history.end(); ++it) {
            QDate d = QDate::fromString(it.key(), "yyyy-MM-dd");
            QDateTime dt(d, QTime(12, 0, 0)); // Ставимо середину дня, щоб точки були по центру дня
            
            // Оптимізація: додаємо точку тільки якщо вона попадає в графік (або трохи з країв)
            if (dt >= range.first.addDays(-1) && dt <= range.second.addDays(1)) {
                x.append(dt.toMSecsSinceEpoch() / 1000.0);
                double val = it.value();
                y.append(val);
                if (val > maxVal) maxVal = val;
            }
        }

        customPlot->addGraph();
        customPlot->graph(i)->setData(x, y);
        customPlot->graph(i)->setName(target->name);

        // Колір
        QColor graphColor;
        if (QColor::isValidColorName(target->color)) graphColor = QColor(target->color);
        else graphColor = NeonPalette::getColor(i);
        
        QPen pen;
        pen.setColor(graphColor);
        pen.setWidth(3);
        customPlot->graph(i)->setPen(pen);

        // Градієнт
        QLinearGradient gradient(0, 0, 0, 400);
        gradient.setColorAt(0, QColor(graphColor.red(), graphColor.green(), graphColor.blue(), 40)); 
        gradient.setColorAt(1, QColor(graphColor.red(), graphColor.green(), graphColor.blue(), 0));   
        customPlot->graph(i)->setBrush(QBrush(gradient));
        
        // Scatter Style (Точки на лінії) - це додасть краси, щоб бачити конкретні дні
        customPlot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::white, 1.5), QBrush(graphColor), 6));
    }

    // 4. Фіналізація
    if (maxVal > 0) customPlot->yAxis->setRange(0, maxVal * 1.2);
    else customPlot->yAxis->setRange(0, 10); // Дефолтний масштаб, якщо пусто
    
    customPlot->legend->setVisible(currentMetricIds.size() > 0);
    customPlot->replot();
}

QPair<QDateTime, QDateTime> ChartPanel::getCurrentRange() const {
    QDateTime start, end;
    
    if (currentMode == Week) {
        // Початок тижня (Понеділок)
        QDate d = currentDate.addDays(-(currentDate.dayOfWeek() - 1));
        start = QDateTime(d, QTime(0, 0, 0));
        end = QDateTime(d.addDays(6), QTime(23, 59, 59));
    } 
    else if (currentMode == Month) {
        start = QDateTime(QDate(currentDate.year(), currentDate.month(), 1), QTime(0, 0, 0));
        end = QDateTime(QDate(currentDate.year(), currentDate.month(), currentDate.daysInMonth()), QTime(23, 59, 59));
    } 
    else { // Year
        start = QDateTime(QDate(currentDate.year(), 1, 1), QTime(0, 0, 0));
        end = QDateTime(QDate(currentDate.year(), 12, 31), QTime(23, 59, 59));
    }
    
    return {start, end};
}

void ChartPanel::setupDarkTheme() {
    // ... (Тут твій старий код теми без змін) ...
    // Тільки видали там рядки про Interactions (Zoom/Drag),
    // бо ми тепер керуємо часом через кнопки, 
    // і ручний зум може зламати нашу логіку відображення дат.
    
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