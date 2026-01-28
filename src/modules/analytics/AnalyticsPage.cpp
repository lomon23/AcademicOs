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
    ChartPanel *chart = new ChartPanel(this);

    // --- ВАЖЛИВО: ЗВ'ЯЗОК (SIGNAL/SLOT) ---
    // Коли тиснеш ⚡ в MetricsPanel -> ChartPanel оновлюється
    connect(metrics, &MetricsPanel::metricSelected, chart, &ChartPanel::updateChart);
    
    // --- 4. РОЗСТАВЛЯЄМО ПО СІТЦІ ---
    
    // Ряд 0: Заголовок
    mainLayout->addWidget(titleLabel, 0, 0, 1, 2);

    // Ряд 1: Лінія
    mainLayout->addWidget(line, 1, 0, 1, 2);

    // Ряд 2: Хедер (Червоний)
    mainLayout->addWidget(header, 2, 0, 1, 2); 

    // Ряд 3: Метрики (Зелені)
    mainLayout->addWidget(metrics, 3, 0);

    // Ряд 3: Графік (Фіолетовий)
    mainLayout->addWidget(chart, 3, 1);
    
    // --- 5. РОЗТЯГУВАННЯ ---
    mainLayout->setRowStretch(3, 1); // Графік тягнеться вниз
    mainLayout->setColumnStretch(0, 0); // Ліва колонка фіксована
    mainLayout->setColumnStretch(1, 1); // Права розтягується
}