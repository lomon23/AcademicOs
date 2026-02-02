#include "AnalyticsPage.h"

// 1. ПІДКЛЮЧАЄМО БІБЛІОТЕКИ
#include <QLabel>
#include <QFrame>

// Підключаємо компоненти
#include "AnalyticsHeader.h"
#include "MetricsPanel.h"
#include "ChartPanel.h"

AnalyticsPage::AnalyticsPage(QWidget *parent) : QWidget(parent) {
    setupLayout();
}

void AnalyticsPage::setupLayout() {
    // Створюємо сітку
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // --- 1. ЗАГОЛОВОК ---
    QLabel *titleLabel = new QLabel("Analytics", this);
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: white;");
    
    // --- 2. ЛІНІЯ (SEPARATOR) ---
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: #333333;");
    line->setFixedHeight(1);

    // --- 3. СТВОРЮЄМО БЛОКИ ---
    AnalyticsHeader *header = new AnalyticsHeader(this);
    MetricsPanel *metrics = new MetricsPanel(this);
    metrics->setFixedWidth(320);
    ChartPanel *chart = new ChartPanel(this);

    // --- ВАЖЛИВО: ЗВ'ЯЗКИ (ВИПРАВЛЕНО ТУТ) ---
    
    // 1. Зміна вкладки (Header -> MetricsPanel)
    // Було: categoryChanged -> refreshMetrics
    // Стало: categorySelected -> setCategory
    connect(header, &AnalyticsHeader::categorySelected, metrics, &MetricsPanel::setCategory);

    // 2. Оновлення даних (MetricsPanel -> ChartPanel)
    connect(metrics, &MetricsPanel::dataChanged, chart, [chart](){ chart->updateChart(); });

    // 3. Клік по метриці (MetricsPanel -> ChartPanel)
    connect(metrics, &MetricsPanel::selectionChanged, chart, &ChartPanel::updateChart);
    // ...

    // --- 4. РОЗСТАВЛЯЄМО ПО СІТЦІ ---
    
    // Ряд 0: Заголовок
    mainLayout->addWidget(titleLabel, 0, 0, 1, 2);

    // Ряд 1: Лінія
    mainLayout->addWidget(line, 1, 0, 1, 2);

    // Ряд 2: Хедер (Кнопки)
    mainLayout->addWidget(header, 2, 0, 1, 2); 

    // Ряд 3: Метрики (Ліворуч)
    mainLayout->addWidget(metrics, 3, 0);

    // Ряд 3: Графік (Праворуч)
    mainLayout->addWidget(chart, 3, 1);
    
    // --- 5. РОЗТЯГУВАННЯ ---
    mainLayout->setRowStretch(3, 1); 
    mainLayout->setColumnStretch(0, 0); 
    mainLayout->setColumnStretch(1, 1); 
}