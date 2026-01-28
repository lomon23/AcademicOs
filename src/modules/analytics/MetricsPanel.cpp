#include "MetricsPanel.h"
#include "../../core/analytics/AnalyticsService.h"

#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include <QScrollArea>
#include <QFrame>
#include <QHBoxLayout>
#include <QDate>
#include <QDebug>

MetricsPanel::MetricsPanel(QWidget *parent) : QWidget(parent) {
    // 1. Основний лейаут
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);

    // --- HEADER (Кнопки) ---
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    // Кнопка Add
    QPushButton *addButton = new QPushButton("+ Add Metric", this);
    addButton->setCursor(Qt::PointingHandCursor);
    addButton->setStyleSheet(
        "QPushButton { background-color: #2E2E2E; color: white; border-radius: 6px; padding: 8px; font-weight: bold; }"
        "QPushButton:hover { background-color: #3E3E3E; }"
    );
    connect(addButton, &QPushButton::clicked, this, &MetricsPanel::onAddMetricClicked);
    
    // Кнопка Gen (⚡)
    QPushButton *genButton = new QPushButton("⚡", this);
    genButton->setToolTip("Generate Mock Data (30 days)");
    genButton->setFixedSize(30, 30);
    genButton->setStyleSheet("background-color: #FFB86C; color: #282a36; border-radius: 6px; font-weight: bold;");
    connect(genButton, &QPushButton::clicked, this, &MetricsPanel::onGenDataClicked);

    headerLayout->addWidget(addButton);
    headerLayout->addWidget(genButton);
    
    // Додаємо хедер у головний лейаут
    mainLayout->addLayout(headerLayout);

    // --- SCROLL AREA ---
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent;");

    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: transparent;");
    contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(8);
    contentLayout->addStretch(); 

    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);
}

// Це новий метод замість refreshMetrics
void MetricsPanel::setCategory(const QString &category) {
    this->currentCategory = category; // Запам'ятовуємо, де ми є

    // 1. Очищаємо старі віджети
    QLayoutItem *item;
    while ((item = contentLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }
    
    // 2. Беремо ТІЛЬКИ метрики цієї категорії (Backend вже відфільтрував)
    auto metrics = AnalyticsService::instance().getMetricsByCategory(category);

    // 3. Малюємо
    for (const auto &m : metrics) {
        QFrame *card = new QFrame();
        card->setStyleSheet("background-color: #1E1E1E; border-radius: 6px;");
        card->setFixedHeight(50);
        
        QHBoxLayout *cardLayout = new QHBoxLayout(card);
        cardLayout->setContentsMargins(10, 5, 10, 5);
        
        // Клік
        QPushButton *btn = new QPushButton(card);
        btn->setStyleSheet("background: transparent; border: none;");
        btn->setGeometry(0, 0, 300, 50);
        connect(btn, &QPushButton::clicked, [this, m]() {
            emit metricSelected(m.id);
        });

        // Назва
        QLabel *nameLabel = new QLabel(m.name, card);
        nameLabel->setStyleSheet("color: white; font-size: 14px; font-weight: bold; border: none; background: transparent;");
        
        // Значення
        QString todayKey = QDate::currentDate().toString("yyyy-MM-dd");
        QString valueText = "No data";
        QString colorStyle = "color: #666;";

        if (m.history.contains(todayKey)) {
            double val = m.history.value(todayKey);
            valueText = QString::number(val, 'f', 1);
            colorStyle = "color: #50FA7B; font-weight: bold;";
        }
        
        QLabel *valueLabel = new QLabel(valueText, card);
        valueLabel->setStyleSheet(colorStyle + "font-size: 14px; border: none; background: transparent;");

        cardLayout->addWidget(nameLabel);
        cardLayout->addStretch();
        cardLayout->addWidget(valueLabel);
        
        contentLayout->addWidget(card);
    }
    contentLayout->addStretch();
}

void MetricsPanel::onAddMetricClicked() {
    bool ok;
    QString name = QInputDialog::getText(this, "New Metric",
                                         "Metric Name:", QLineEdit::Normal,
                                         "", &ok);
    
    if (ok && !name.isEmpty()) {
        // !!! ТУТ БУЛА ПОМИЛКА: Тепер передаємо категорію !!!
        AnalyticsService::instance().createMetric(name, currentCategory);
        
        // Оновлюємо список поточної категорії
        setCategory(currentCategory); 

        emit dataChanged();
    }
}

void MetricsPanel::onGenDataClicked() {
    AnalyticsService::instance().generateMockData();
    setCategory(currentCategory); // Оновлюємо UI
    emit dataChanged();
}