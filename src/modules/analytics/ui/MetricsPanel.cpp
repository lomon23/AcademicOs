#include "src/modules/analytics/ui/MetricsPanel.h"
#include "src/modules/analytics/core/AnalyticsService.h"
#include <QLineEdit>        // <--- Перевір
#include <QDoubleSpinBox>   // <--- ДОДАЙ ЦЕ

#include <QTimer>
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
#include <QFormLayout>

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
    showInlineEditor();
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
            AnalyticsService::instance().updateMetricDetails(
                metricId, 
                currentMetric.name, 
                newColor.name(), 
                currentMetric.units, 
                currentMetric.minVal, // <--- Додали
                currentMetric.maxVal  // <--- Додали
            );
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
            AnalyticsService::instance().updateMetricDetails(
                metricId, 
                name, 
                currentMetric.color, 
                currentMetric.units,
                currentMetric.minVal, // <--- Додали
                currentMetric.maxVal  // <--- Додали
            );
            setCategory(currentCategory);
        }
    }
}

void MetricsPanel::showInlineEditor() {
    // 1. Створюємо контейнер (виглядає як картка, але "роздута")
    QFrame *editor = new QFrame();
    editor->setStyleSheet("background-color: #2A2A2A; border: 1px solid #BD93F9; border-radius: 8px;");
    editor->setFixedHeight(170); // Більша висота для налаштувань

    QVBoxLayout *layout = new QVBoxLayout(editor);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    // --- РЯДОК 1: Назва ---
    QLineEdit *nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Metric Name (e.g. Sleep)");
    nameEdit->setStyleSheet("background: #1E1E1E; color: white; border: 1px solid #444; padding: 5px;");
    nameEdit->setFocus(); // Одразу ставимо курсор сюди

    // --- РЯДОК 2: Одиниці та Колір ---
    QHBoxLayout *row2 = new QHBoxLayout();
    
    QLineEdit *unitsEdit = new QLineEdit();
    unitsEdit->setPlaceholderText("Units (h, kg)");
    unitsEdit->setStyleSheet("background: #1E1E1E; color: white; border: 1px solid #444; padding: 5px;");
    
    QPushButton *colorBtn = new QPushButton();
    colorBtn->setFixedSize(30, 30);
    QString initColor = "#BD93F9";
    colorBtn->setStyleSheet(QString("background-color: %1; border: none; border-radius: 4px;").arg(initColor));
    colorBtn->setProperty("color", initColor); // Зберігаємо колір у властивості

    // Логіка вибору кольору (все ж діалог, але маленький)
    connect(colorBtn, &QPushButton::clicked, [this, colorBtn]() {
        QColor c = QColorDialog::getColor(QColor(colorBtn->property("color").toString()), this);
        if (c.isValid()) {
            colorBtn->setStyleSheet(QString("background-color: %1; border: none; border-radius: 4px;").arg(c.name()));
            colorBtn->setProperty("color", c.name());
        }
    });

    row2->addWidget(unitsEdit);
    row2->addWidget(new QLabel("Color:"));
    row2->addWidget(colorBtn);

    // --- РЯДОК 3: Межі (Min/Max) ---
    QHBoxLayout *row3 = new QHBoxLayout();
    
    QDoubleSpinBox *minSpin = new QDoubleSpinBox();
    minSpin->setRange(-9999, 9999);
    minSpin->setPrefix("Min: ");
    minSpin->setButtonSymbols(QAbstractSpinBox::NoButtons);
    minSpin->setStyleSheet("background: #1E1E1E; color: #AAA; border: 1px solid #444;");

    QDoubleSpinBox *maxSpin = new QDoubleSpinBox();
    maxSpin->setRange(-9999, 9999);
    maxSpin->setPrefix("Max: ");
    maxSpin->setButtonSymbols(QAbstractSpinBox::NoButtons);
    maxSpin->setStyleSheet("background: #1E1E1E; color: #AAA; border: 1px solid #444;");

    row3->addWidget(minSpin);
    row3->addWidget(maxSpin);

    // --- ЛОГІКА ЗБЕРЕЖЕННЯ ---
    auto saveFunc = [this, nameEdit, unitsEdit, colorBtn, minSpin, maxSpin]() {
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) return; // Не зберігаємо пусте

        // 1. Створюємо метрику (це безпечно)
        AnalyticsService::instance().createMetric(
            name,
            currentCategory,
            colorBtn->property("color").toString(),
            unitsEdit->text(),
            minSpin->value(),
            maxSpin->value()
        );
        
        // 2. ВАЖЛИВИЙ ФІКС: 
        // Використовуємо таймер, щоб оновити інтерфейс ТІЛЬКИ після того, 
        // як кнопка завершить свою роботу. Це прибере Segmentation Fault.
        QTimer::singleShot(0, this, [this](){
            setCategory(currentCategory); 
            emit dataChanged();
        });
    };

    // Зберігаємо по Enter на назві (для швидкості)
    connect(nameEdit, &QLineEdit::returnPressed, saveFunc);
    
    // Або кнопка "Save" (галочка)
    QPushButton *saveBtn = new QPushButton("✔ Save");
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet("background: #50FA7B; color: black; font-weight: bold; border-radius: 4px;");
    connect(saveBtn, &QPushButton::clicked, saveFunc);

    // Кнопка "Cancel"
    QPushButton *cancelBtn = new QPushButton("✖");
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setFixedSize(30, 30);
    cancelBtn->setStyleSheet("background: transparent; color: #FF5555; font-weight: bold;");
    connect(cancelBtn, &QPushButton::clicked, [this](){
        setCategory(currentCategory); // Просто оновлюємо список, редактор зникне
    });

    QHBoxLayout *actionRow = new QHBoxLayout();
    actionRow->addStretch();
    actionRow->addWidget(cancelBtn);
    actionRow->addWidget(saveBtn);

    // Збираємо все у фрейм
    layout->addWidget(nameEdit);
    layout->addLayout(row2);
    layout->addLayout(row3);
    layout->addLayout(actionRow);

    // Вставляємо на початок списку!
    contentLayout->insertWidget(0, editor);
}