#include "AnalyticsFullPage.h"
#include <QVBoxLayout>
#include <QLabel>

AnalyticsFullPage::AnalyticsFullPage(QWidget *parent) : QWidget(parent), chartTitleElement(nullptr)
{
    // --- ФІКС ЛЕЙАУТУ (Розтягуємо графік) ---
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); // Прибираємо відступи по краях вікна
    layout->setSpacing(0); // Прибираємо проміжки між елементами

    // 1. Верхня панелька для назви (з відступами, щоб було красиво)
    QWidget *topBar = new QWidget(this);
    topBar->setStyleSheet("background-color: #1E1E1E; border-bottom: 1px solid #333;");
    topBar->setFixedHeight(60); // Фіксована висота шапки
    
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(20, 10, 20, 10);
    
    topLayout->addWidget(new QLabel("Widget Title:", topBar));
    
    titleInput = new QLineEdit(topBar);
    titleInput->setPlaceholderText("Enter chart name...");
    // Стиль поля вводу
    titleInput->setStyleSheet(
        "QLineEdit { "
        "  padding: 5px 10px; "
        "  background: #2C2C2C; "
        "  color: white; "
        "  border: 1px solid #444; "
        "  border-radius: 6px; "
        "  font-size: 14px; "
        "}"
        "QLineEdit:focus { border: 1px solid #00E676; }"
    );
    topLayout->addWidget(titleInput);
    
    layout->addWidget(topBar);

    // 2. Графік (Додаємо з коефіцієнтом розтягування 1)
    chart = new QCustomPlot(this);
    // chart->setMinimumHeight(400); // Це більше не треба, він сам розтягнеться
    layout->addWidget(chart, 1); // <--- Одиниця важлива! Вона каже "займай все місце"
    
    setupChart();

    connect(titleInput, &QLineEdit::textChanged, this, &AnalyticsFullPage::onTitleChanged);
}

void AnalyticsFullPage::setTitle(const QString &title) {
    titleInput->setText(title); 
    // Оновлюємо і на графіку (щоб при старті було видно)
    if (chartTitleElement) {
        chartTitleElement->setText(title);
        chart->replot();
    }
}

void AnalyticsFullPage::setData(const QVector<double> &data) {
    QVector<double> x(data.size());
    for (int i = 0; i < data.size(); ++i) x[i] = i;

    chart->graph(0)->setData(x, data);
    chart->rescaleAxes();
    chart->replot();
}

void AnalyticsFullPage::onTitleChanged(const QString &text) {
    emit configChanged(text);
    
    // --- ФІКС: ПРОСТО ОНОВЛЮЄМО ТЕКСТ ---
    // Не додаємо новий рядок, а змінюємо існуючий
    if (chartTitleElement) {
        chartTitleElement->setText(text);
        chart->replot();
    }
}

void AnalyticsFullPage::setupChart() {
    chart->addGraph();
    chart->graph(0)->setPen(QPen(QColor("#00E676"), 3));
    chart->setBackground(Qt::NoBrush);
    
    chart->xAxis->setBasePen(QPen(Qt::white));
    chart->yAxis->setBasePen(QPen(Qt::white));
    chart->xAxis->setTickLabelColor(Qt::white);
    chart->yAxis->setTickLabelColor(Qt::white);
    
    // --- ФІКС ЗАГОЛОВКА ---
    // Створюємо його ОДИН раз тут
    chart->plotLayout()->insertRow(0);
    chartTitleElement = new QCPTextElement(chart, "", QFont("sans", 14, QFont::Bold));
    chartTitleElement->setTextColor(QColor(200, 200, 200));
    chart->plotLayout()->addElement(0, 0, chartTitleElement);
}

