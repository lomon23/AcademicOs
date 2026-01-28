#include "MetricsPanel.h"
#include "../../core/analytics/AnalyticsService.h" // Підключаємо наш бекенд

#include <QPushButton>
#include <QLabel>
#include <QInputDialog> // Для віконця введення тексту
#include <QScrollArea>
#include <QFrame>
#include <QDebug>

MetricsPanel::MetricsPanel(QWidget *parent) : QWidget(parent) {
    // 1. Основний лейаут панелі
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);

    // --- HEADER (Кнопка Додати) ---
    QPushButton *addButton = new QPushButton("+ Add Metric", this);
    addButton->setCursor(Qt::PointingHandCursor);
    // Стиль кнопки (можна потім винести в CSS)
    addButton->setStyleSheet(
        "QPushButton { background-color: #2E2E2E; color: white; border-radius: 6px; padding: 8px; font-weight: bold; }"
        "QPushButton:hover { background-color: #3E3E3E; }"
    );
    connect(addButton, &QPushButton::clicked, this, &MetricsPanel::onAddMetricClicked);
    mainLayout->addWidget(addButton);

    // --- SCROLL AREA (Щоб список прокручувався, якщо метрик багато) ---
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent;"); // Прозорий фон

    // Контейнер всередині скролу
    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: transparent;");
    contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(8);
    contentLayout->addStretch(); // Пружина знизу, щоб віджети притискались до верху

    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);
    QPushButton *genButton = new QPushButton("⚡", this);
    genButton->setToolTip("Generate Mock Data (30 days)");
    genButton->setFixedSize(30, 30); // Маленька квадратна кнопка
    genButton->setStyleSheet("background-color: #FFB86C; color: #282a36; border-radius: 6px; font-weight: bold;");
    connect(genButton, &QPushButton::clicked, this, &MetricsPanel::onGenDataClicked);

    // Щоб кнопки були в один ряд, зробимо для них маленький лейаут
    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->addWidget(addButton);   // Велика кнопка Add
    headerLayout->addWidget(genButton);   // Мала кнопка Gen
    
    // Додаємо цей лейаут в mainLayout замість просто addButton
    mainLayout->addLayout(headerLayout);
    // 2. Завантажуємо дані при старті
    refreshMetrics();
}

void MetricsPanel::onAddMetricClicked() {
    bool ok;
    // Викликаємо просте віконце введення тексту
    QString name = QInputDialog::getText(this, "New Metric",
                                         "Metric Name (e.g. 'Sleep', 'Pushups'):", QLineEdit::Normal,
                                         "", &ok);
    
    if (ok && !name.isEmpty()) {
        // 1. Створюємо в бекенді
        AnalyticsService::instance().createMetric(name);
        
        // 2. Оновлюємо список на екрані
        refreshMetrics(); 

        emit dataChanged();
    }
}

void MetricsPanel::refreshMetrics() {
    // 1. Очищаємо старі віджети (крім пружини в кінці)
    QLayoutItem *item;
    while ((item = contentLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget(); // Видаляємо фізично кнопку/лейбл
        }
        delete item;
    }
    // (Пружину треба додати заново або не видаляти, тут ми просто очистили все)
    
    // 2. Беремо список з бази
    auto metrics = AnalyticsService::instance().getAllMetrics();

    // 3. Малюємо кожен елемент
    for (const auto &m : metrics) {
        QFrame *card = new QFrame();
        card->setStyleSheet("background-color: #1E1E1E; border-radius: 6px;");
        card->setFixedHeight(50);
        
        QHBoxLayout *cardLayout = new QHBoxLayout(card);
        cardLayout->setContentsMargins(10, 5, 10, 5);
        
        // --- ПРОЗОРА КНОПКА ДЛЯ КЛІКУ ---
        QPushButton *btn = new QPushButton(card);
        btn->setStyleSheet("background: transparent; border: none;");
        btn->setGeometry(0, 0, 300, 50);
        connect(btn, &QPushButton::clicked, [this, m]() {
            emit metricSelected(m.id);
        });
        // -------------------------------

        // Назва
        QLabel *nameLabel = new QLabel(m.name, card);
        nameLabel->setStyleSheet("color: white; font-size: 14px; font-weight: bold; border: none; background: transparent;");
        
        // --- ОТРИМУЄМО ЗНАЧЕННЯ ЗА СЬОГОДНІ ---
        QString todayKey = QDate::currentDate().toString("yyyy-MM-dd");
        QString valueText = "No data";
        QString colorStyle = "color: #666;"; // Сірий колір для "No data"

        if (m.history.contains(todayKey)) {
            double val = m.history.value(todayKey);
            valueText = QString::number(val, 'f', 1); // Округлення до 1 знаку (напр. 8.5)
            colorStyle = "color: #50FA7B; font-weight: bold;"; // Яскраво-зелений для цифри
        }
        
        QLabel *valueLabel = new QLabel(valueText, card);
        valueLabel->setStyleSheet(colorStyle + "font-size: 14px; border: none; background: transparent;");
        // ---------------------------------------

        cardLayout->addWidget(nameLabel);
        cardLayout->addStretch();
        cardLayout->addWidget(valueLabel);
        
        contentLayout->addWidget(card);
    }
    contentLayout->addStretch(); // Повертаємо пружину в кінець
}

void MetricsPanel::onGenDataClicked() {
    // 1. Генеруємо дані
    AnalyticsService::instance().generateMockData();
    
    // 2. Оновлюємо список (хоча цифри ми поки не показуємо, але це корисно для дебагу)
    refreshMetrics();

    emit dataChanged();
}
