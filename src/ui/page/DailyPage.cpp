#include "DailyPage.h"
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QDebug>

DailyPage::DailyPage(QWidget *parent) : QWidget(parent)
{
    setupUi();
}

void DailyPage::setupUi() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(20);

    // 1. Заголовок
    QLabel *title = new QLabel("Daily Check-in", this);
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: white;");
    mainLayout->addWidget(title);

    dateLabel = new QLabel(this);
    dateLabel->setStyleSheet("font-size: 16px; color: #AAAAAA; margin-bottom: 20px;");
    mainLayout->addWidget(dateLabel);

    // 2. Форма (Сюди будемо додавати поля динамічно)
    formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignLeft);
    formLayout->setVerticalSpacing(20);
    formLayout->setHorizontalSpacing(40);
    mainLayout->addLayout(formLayout);

    mainLayout->addStretch(); // Пружина, щоб кнопка була внизу

    // 3. Кнопка Save
    QPushButton *saveBtn = new QPushButton("Complete Check-in", this);
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setFixedHeight(50);
    saveBtn->setStyleSheet(
        "QPushButton { background-color: #00E676; color: #121212; border-radius: 8px; font-weight: bold; font-size: 16px; }"
        "QPushButton:hover { background-color: #69F0AE; }"
    );
    connect(saveBtn, &QPushButton::clicked, this, &DailyPage::onSaveClicked);
    mainLayout->addWidget(saveBtn);
}

void DailyPage::prepareForShow() {
    // Оновлюємо дату
    dateLabel->setText("For " + QDate::currentDate().toString("dddd, MMMM d, yyyy"));
    
    // Перебудовуємо форму (раптом додалися нові метрики)
    buildForm();
}

void DailyPage::buildForm() {
    // Безпечне очищення лейауту
    QLayoutItem *item;
    while ((item = formLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    inputs.clear();

    // Отримуємо список метрик з "Мозку"
    auto metrics = MetricManager::instance().getAllDefinitions();

    for (const auto &def : metrics) {
        QWidget *inputWidget = nullptr;

        // СТВОРЮЄМО ВІДЖЕТ В ЗАЛЕЖНОСТІ ВІД ТИПУ
        switch (def.type) {
            case MetricType::Number: {
                QSpinBox *spin = new QSpinBox();
                spin->setRange((int)def.min, (int)def.max);
                spin->setStyleSheet("background: #333; color: white; padding: 5px; border-radius: 4px;");
                spin->setSuffix(" " + def.unit);
                inputWidget = spin;
                break;
            }
            case MetricType::Money: {
                // Створюємо контейнер для "Звірки балансу"
                QWidget *moneyContainer = new QWidget();
                QVBoxLayout *vLayout = new QVBoxLayout(moneyContainer);
                vLayout->setContentsMargins(0,0,0,0);
                vLayout->setSpacing(5);

                // 1. Інформація: Скільки має бути
                QLabel *infoLabel = new QLabel(QString("System Balance: %1 %2").arg(currentBalance).arg(def.unit));
                infoLabel->setStyleSheet("color: #888; font-size: 12px;");
                vLayout->addWidget(infoLabel);

                // 2. Ввід: Скільки є насправді
                QDoubleSpinBox *spin = new QDoubleSpinBox();
                spin->setRange(0, 1000000); // Від 0 до мільйона
                spin->setValue(currentBalance); // За замовчуванням ставимо поточний баланс
                spin->setSuffix(" " + def.unit);
                spin->setStyleSheet("background: #333; color: white; padding: 8px; border-radius: 4px; font-weight: bold; font-size: 14px;");
                vLayout->addWidget(spin);

                // 3. Підказка: Скільки витрачено (Різниця)
                QLabel *diffLabel = new QLabel("Spent today: 0 " + def.unit);
                diffLabel->setStyleSheet("color: #00E676; font-weight: bold; font-size: 13px;");
                vLayout->addWidget(diffLabel);

                // Живий перерахунок при зміні значення
                connect(spin, &QDoubleSpinBox::valueChanged, [this, diffLabel, def](double newVal){
                    double diff = currentBalance - newVal;
                    if (diff >= 0) {
                        diffLabel->setText(QString("Spent today: %1 %2").arg(diff).arg(def.unit));
                        diffLabel->setStyleSheet("color: #FF5252;"); // Червоний (Витрата)
                    } else {
                        diffLabel->setText(QString("Income today: %1 %2").arg(-diff).arg(def.unit));
                        diffLabel->setStyleSheet("color: #00E676;"); // Зелений (Дохід/Знайшов гроші)
                    }
                });

                inputWidget = spin; // Зберігаємо спінбокс як основний інпут для зчитування
                
                // Додаємо в форму контейнер, а не просто спінбокс
                QLabel *label = new QLabel(def.name + " (Actual)");
                label->setStyleSheet("color: #DDD; font-size: 14px; margin-top: 10px;");
                formLayout->addRow(label, moneyContainer);
                
                inputs[def.id] = {def.type, spin};
                continue; // Пропускаємо стандартне додавання
            }
            case MetricType::Slider: {
                // Для слайдера робимо контейнер (Слайдер + Цифра збоку)
                QWidget *container = new QWidget();
                QHBoxLayout *hLayout = new QHBoxLayout(container);
                hLayout->setContentsMargins(0,0,0,0);
                
                QSlider *slider = new QSlider(Qt::Horizontal);
                slider->setRange((int)def.min, (int)def.max);
                
                QLabel *valLabel = new QLabel(QString::number(def.min));
                valLabel->setFixedWidth(30);
                valLabel->setStyleSheet("color: white; font-weight: bold;");

                connect(slider, &QSlider::valueChanged, [valLabel](int v){
                    valLabel->setText(QString::number(v));
                });

                hLayout->addWidget(slider);
                hLayout->addWidget(valLabel);
                
                // Зберігаємо сам слайдер як інпут, а не контейнер
                inputWidget = slider; 
                
                // Але в форму додаємо контейнер
                QLabel *label = new QLabel(def.name);
                label->setStyleSheet("color: #DDD; font-size: 14px;");
                formLayout->addRow(label, container);
                
                inputs[def.id] = {def.type, slider}; 
                continue; // Особливий випадок, вже додали в форму
            }
            case MetricType::Boolean: {
                QCheckBox *check = new QCheckBox();
                check->setStyleSheet("QCheckBox { color: white; }");
                inputWidget = check;
                break;
            }
        }

        if (inputWidget) {
            // Стилізація підпису
            QLabel *label = new QLabel(def.name);
            label->setStyleSheet("color: #DDD; font-size: 14px;");
            
            formLayout->addRow(label, inputWidget);
            inputs[def.id] = {def.type, inputWidget};
        }
    }
}

void DailyPage::onSaveClicked() {
    QDate today = QDate::currentDate();

    for (auto it = inputs.begin(); it != inputs.end(); ++it) {
        QString id = it.key();
        InputField field = it.value();
        QVariant valueToLog;

        switch (field.type) {
            case MetricType::Money: {
                double actualBalance = static_cast<QDoubleSpinBox*>(field.widget)->value();
                double spent = currentBalance - actualBalance; // 3130 - 3000 = 130 (Витратив)
                
                valueToLog = spent; 
                
                // Якщо витрата не нульова, треба оновити і реальний гаманець!
                if (spent != 0) {
                    emit walletCorrection(spent);
                }
                break;
            }
            // ... інші кейси без змін ...
            case MetricType::Number:
                valueToLog = static_cast<QSpinBox*>(field.widget)->value();
                break;
            case MetricType::Slider:
                valueToLog = static_cast<QSlider*>(field.widget)->value();
                break;
            case MetricType::Boolean:
                valueToLog = static_cast<QCheckBox*>(field.widget)->isChecked();
                break;
        }

        MetricManager::instance().logValue(id, today, valueToLog);
    }

    qDebug() << "Daily data saved & processed!";
    emit finished();
}
void DailyPage::setWalletBalance(double amount) {
    currentBalance = amount;
}