#include "MetricsPanel.h"
#include "../../core/analytics/AnalyticsService.h"

#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include <QScrollArea>
#include <QFrame>
#include <QHBoxLayout>
#include <QDate>
#include <QMenu>
#include <QAction>
#include <QColorDialog>
#include <QDebug>

MetricsPanel::MetricsPanel(QWidget *parent) : QWidget(parent) {
    // 1. Основний лейаут
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);

    // --- HEADER ---
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    QPushButton *addButton = new QPushButton("+ Add Metric", this);
    addButton->setCursor(Qt::PointingHandCursor);
    addButton->setStyleSheet(
        "QPushButton { background-color: #2E2E2E; color: white; border-radius: 6px; padding: 8px; font-weight: bold; }"
        "QPushButton:hover { background-color: #3E3E3E; }"
    );
    connect(addButton, &QPushButton::clicked, this, &MetricsPanel::onAddMetricClicked);
    
    QPushButton *genButton = new QPushButton("⚡", this);
    genButton->setToolTip("Generate Mock Data");
    genButton->setFixedSize(30, 30);
    genButton->setStyleSheet("background-color: #FFB86C; color: #282a36; border-radius: 6px; font-weight: bold;");
    connect(genButton, &QPushButton::clicked, this, &MetricsPanel::onGenDataClicked);

    headerLayout->addWidget(addButton);
    headerLayout->addWidget(genButton);
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

void MetricsPanel::setCategory(const QString &category) {
    this->currentCategory = category;
    
    // 1. Очищаємо список вибраних
    selectedMetrics.clear();
    emit selectionChanged(selectedMetrics);

    // 2. Чистимо UI
    QLayoutItem *item;
    while ((item = contentLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }
    
    // 3. Отримуємо метрики
    auto metrics = AnalyticsService::instance().getMetricsByCategory(category);

    for (const auto &m : metrics) {
        // --- СТВОРЕННЯ КАРТКИ ---
        QPushButton *card = new QPushButton();
        card->setCheckable(true); 
        card->setFixedHeight(60); 
        card->setCursor(Qt::PointingHandCursor);

        // Вмикаємо Контекстне меню (ПКМ)
        card->setContextMenuPolicy(Qt::CustomContextMenu);
        card->setProperty("metricId", m.id); // Зберігаємо ID всередині кнопки
        connect(card, &QPushButton::customContextMenuRequested, this, &MetricsPanel::onContextMenuRequested);

        // Стиль
        card->setStyleSheet(
            "QPushButton { "
            "  background-color: #1E1E1E; "
            "  border: 1px solid #333; "
            "  border-radius: 8px; "
            "}"
            "QPushButton:hover { background-color: #252525; }"
            "QPushButton:checked { "
            "  background-color: rgba(80, 250, 123, 0.1); " 
            "  border: 2px solid #50FA7B; "                 
            "}"
        );

        QHBoxLayout *cardLayout = new QHBoxLayout(card);
        cardLayout->setContentsMargins(15, 0, 15, 0);

        // Назва
        QLabel *nameLabel = new QLabel(m.name);
        nameLabel->setStyleSheet("color: white; font-size: 14px; font-weight: bold; border: none; background: transparent;");

        // Значення
        QString todayKey = QDate::currentDate().toString("yyyy-MM-dd");
        QString valueText = "-";
        QString valueColor = "color: #666;";

        if (m.history.contains(todayKey)) {
             double val = m.history.value(todayKey);
             if (!m.units.isEmpty()) {
                 valueText = QString::number(val, 'f', 1) + " " + m.units;
             } else {
                 valueText = QString::number(val, 'f', 1);
             }
             valueColor = "color: #50FA7B; font-weight: bold;";
        }
        
        QLabel *valueLabel = new QLabel(valueText);
        valueLabel->setStyleSheet(valueColor + "font-size: 14px; border: none; background: transparent;");

        cardLayout->addWidget(nameLabel);
        cardLayout->addStretch();
        cardLayout->addWidget(valueLabel);
        
        // Логіка кліку (вибір)
        connect(card, &QPushButton::toggled, [this, m](bool checked) {
            if (checked) {
                selectedMetrics.append(m.id);
            } else {
                selectedMetrics.removeAll(m.id);
            }
            emit selectionChanged(selectedMetrics);
        });

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
        AnalyticsService::instance().createMetric(name, currentCategory);
        setCategory(currentCategory); 
        emit dataChanged();
    }
}

void MetricsPanel::onGenDataClicked() {
    AnalyticsService::instance().generateMockData();
    setCategory(currentCategory);
    emit dataChanged();
}

// --- ЛОГІКА КОНТЕКСТНОГО МЕНЮ ---
void MetricsPanel::onContextMenuRequested(const QPoint &pos) {
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn) return;
    
    QString metricId = btn->property("metricId").toString();
    
    // Знаходимо поточну метрику
    auto metrics = AnalyticsService::instance().getAllMetrics();
    Metric currentMetric;
    for (const auto &m : metrics) {
        if (m.id == metricId) {
            currentMetric = m;
            break;
        }
    }
    if (currentMetric.id.isEmpty()) return;

    // Меню
    QMenu menu(this);
    menu.setStyleSheet("QMenu { background-color: #2E2E2E; color: white; border: 1px solid #444; }"
                       "QMenu::item:selected { background-color: #50FA7B; color: black; }");

    QAction *editAction = menu.addAction("✏️ Edit Name / Units");
    QAction *colorAction = menu.addAction("🎨 Change Color");
    menu.addSeparator();
    QAction *deleteAction = menu.addAction("🗑 Delete");

    QAction *selectedItem = menu.exec(btn->mapToGlobal(pos));

    // Обробка
    if (selectedItem == deleteAction) {
        AnalyticsService::instance().deleteMetric(metricId);
        setCategory(currentCategory);
        emit selectionChanged(QStringList());
    }
    else if (selectedItem == colorAction) {
        QColor initColor = QColor::isValidColorName(currentMetric.color) ? QColor(currentMetric.color) : Qt::white;
        QColor newColor = QColorDialog::getColor(initColor, this, "Select Metric Color");
        
        if (newColor.isValid()) {
            AnalyticsService::instance().updateMetricDetails(metricId, currentMetric.name, newColor.name(), currentMetric.units);
            setCategory(currentCategory); 
            emit selectionChanged(selectedMetrics);
        }
    }
    else if (selectedItem == editAction) {
        bool ok;
        QString text = QInputDialog::getText(this, "Edit Metric",
                                             "Format: Name (Units)", QLineEdit::Normal,
                                             currentMetric.name + (currentMetric.units.isEmpty() ? "" : " (" + currentMetric.units + ")"), &ok);
        if (ok && !text.isEmpty()) {
            QString name = text;
            QString units = "";
            if (text.contains("(") && text.contains(")")) {
                int start = text.lastIndexOf("(");
                int end = text.lastIndexOf(")");
                name = text.left(start).trimmed();
                units = text.mid(start + 1, end - start - 1);
            }
            AnalyticsService::instance().updateMetricDetails(metricId, name, currentMetric.color, units);
            setCategory(currentCategory);
        }
    }
}