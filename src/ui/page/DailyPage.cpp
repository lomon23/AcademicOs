#include "DailyPage.h"
#include "../../core/analytics/AnalyticsService.h"

#include <QPushButton>
#include <QScrollArea>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QDebug>

DailyPage::DailyPage(QWidget *parent) : QWidget(parent) {
    currentDate = QDate::currentDate();

    // === ГОЛОВНИЙ ЛЕЙАУТ ===
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    // === 1. HEADER (Навігація Часу) ===
    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(15);
    
    // Кнопка "Назад"
    QPushButton *prevBtn = new QPushButton("<", this);
    prevBtn->setFixedSize(40, 40);
    prevBtn->setCursor(Qt::PointingHandCursor);
    prevBtn->setStyleSheet("background: #333; color: white; border-radius: 8px; font-size: 18px; font-weight: bold;");
    
    // Дата (по центру)
    dateLabel = new QLabel(this);
    dateLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: white;");
    dateLabel->setAlignment(Qt::AlignCenter);
    dateLabel->setMinimumWidth(200);
    
    // Кнопка "Вперед"
    QPushButton *nextBtn = new QPushButton(">", this);
    nextBtn->setFixedSize(40, 40);
    nextBtn->setCursor(Qt::PointingHandCursor);
    nextBtn->setStyleSheet("background: #333; color: white; border-radius: 8px; font-size: 18px; font-weight: bold;");

    // Кнопка "Сьогодні"
    QPushButton *todayBtn = new QPushButton("Today", this);
    todayBtn->setCursor(Qt::PointingHandCursor);
    todayBtn->setFixedHeight(40);
    todayBtn->setStyleSheet("background-color: #BD93F9; color: black; font-weight: bold; border-radius: 8px; padding: 0 15px;");

    headerLayout->addStretch();
    headerLayout->addWidget(prevBtn);
    headerLayout->addWidget(dateLabel);
    headerLayout->addWidget(nextBtn);
    headerLayout->addWidget(todayBtn);
    headerLayout->addStretch();

    mainLayout->addLayout(headerLayout);

    // === 2. ОБЛАСТЬ МЕТРИК (Scroll Area) ===
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent;");

    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: transparent;");
    metricsLayout = new QVBoxLayout(scrollContent);
    metricsLayout->setSpacing(15);
    metricsLayout->setAlignment(Qt::AlignTop); // Притискаємо до верху

    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);

    // === CONNECTIONS ===
    connect(prevBtn, &QPushButton::clicked, this, &DailyPage::onPrevDay);
    connect(nextBtn, &QPushButton::clicked, this, &DailyPage::onNextDay);
    connect(todayBtn, &QPushButton::clicked, this, &DailyPage::onToday);

    updateDateLabel();
    buildForm();
}

void DailyPage::updateDateLabel() {
    if (currentDate == QDate::currentDate()) {
        dateLabel->setText("Today");
        dateLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #50FA7B;"); // Зелений для сьогодні
    } else {
        dateLabel->setText(currentDate.toString("ddd, dd MMM yyyy"));
        dateLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: white;");
    }
}

void DailyPage::onPrevDay() {
    currentDate = currentDate.addDays(-1);
    updateDateLabel();
    buildForm(); // Перебудовуємо форму для нової дати
}

void DailyPage::onNextDay() {
    currentDate = currentDate.addDays(1);
    updateDateLabel();
    buildForm();
}

void DailyPage::onToday() {
    currentDate = QDate::currentDate();
    updateDateLabel();
    buildForm();
}

void DailyPage::refreshData() {
    // Коли відкриваємо сторінку - оновлюємо (раптом додали нову метрику в Analytics)
    buildForm();
}

// === ГЕНЕРАЦІЯ ФОРМИ ===
void DailyPage::buildForm() {
    // 1. Очищаємо старі віджети
    QLayoutItem *item;
    while ((item = metricsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    // 2. Отримуємо дані
    auto categories = AnalyticsService::instance().getCategories();
    auto metrics = AnalyticsService::instance().getAllMetrics();

    if (categories.isEmpty()) {
        QLabel *empty = new QLabel("No metrics found.\nGo to Analytics page to add metrics first!");
        empty->setStyleSheet("color: #666; font-size: 16px; padding: 40px;");
        empty->setAlignment(Qt::AlignCenter);
        metricsLayout->addWidget(empty);
        return;
    }

    // 3. Будуємо по категоріях
    for (const QString &cat : categories) {
        // Знаходимо метрики для цієї категорії
        std::vector<Metric> catMetrics;
        for (const auto &m : metrics) {
            if (m.category == cat) catMetrics.push_back(m);
        }

        if (catMetrics.empty()) continue;

        // --- Заголовок Категорії ---
        QLabel *catHeader = new QLabel(cat);
        catHeader->setStyleSheet("color: #BD93F9; font-size: 16px; font-weight: bold; margin-top: 10px; text-transform: uppercase;");
        metricsLayout->addWidget(catHeader);

        // --- Контейнер (Картка) ---
        QFrame *card = new QFrame();
        card->setStyleSheet("background-color: #1E1E1E; border-radius: 12px; border: 1px solid #2A2A2A;");
        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setSpacing(10);
        cardLayout->setContentsMargins(15, 15, 15, 15);

        for (const auto &m : catMetrics) {
            QHBoxLayout *row = new QHBoxLayout();
            
            // Назва
            QLabel *nameLabel = new QLabel(m.name);
            nameLabel->setStyleSheet("color: #E0E0E0; font-size: 15px; border: none; background: transparent;");
            
            // Поле вводу
            QDoubleSpinBox *input = new QDoubleSpinBox();
            input->setRange(0, 99999);
            input->setDecimals(1); // 1 знак після коми (напр. 5.5 км)
            input->setButtonSymbols(QAbstractSpinBox::NoButtons); // Без стрілочок (чисто)
            input->setAlignment(Qt::AlignRight);
            input->setFixedWidth(100);
            input->setFixedHeight(35);
            
            // Стиль інпута
            input->setStyleSheet(
                "QDoubleSpinBox { background: #252525; color: white; border: 1px solid #333; border-radius: 6px; padding: 0 10px; font-weight: bold; }"
                "QDoubleSpinBox:focus { border: 1px solid #BD93F9; background: #2A2A2A; }"
            );

            // Встановлюємо значення з історії (якщо є)
            QString dateKey = currentDate.toString("yyyy-MM-dd");
            if (m.history.contains(dateKey)) {
                input->setValue(m.history.value(dateKey));
            }

            // Одиниці виміру (якщо є)
            if (!m.units.isEmpty()) {
                input->setSuffix(" " + m.units);
            }

            // --- ЗБЕРЕЖЕННЯ (Auto-Save) ---
            connect(input, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [m, this](double val){
                AnalyticsService::instance().updateValue(m.id, currentDate, val);
            });

            row->addWidget(nameLabel);
            row->addStretch();
            row->addWidget(input);
            cardLayout->addLayout(row);

            // Розділювач (лінія), якщо не останній
            if (m.id != catMetrics.back().id) {
                QFrame *line = new QFrame();
                line->setFixedHeight(1);
                line->setStyleSheet("background: #2A2A2A; border: none;");
                cardLayout->addWidget(line);
            }
        }
        
        metricsLayout->addWidget(card);
    }
    
    metricsLayout->addStretch(); // Пружина внизу
}
void DailyPage::onDateSelected() {
    // Поки що пуста функція. 
    // В майбутньому тут буде логіка вибору дати через віджет календаря.
}