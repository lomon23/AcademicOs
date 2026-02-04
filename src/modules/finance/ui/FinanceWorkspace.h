#ifndef FINANCEWORKSPACE_H
#define FINANCEWORKSPACE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QScrollArea>
#include <QDateEdit> 

class FinanceWorkspace : public QWidget {
    Q_OBJECT
public:
    explicit FinanceWorkspace(QWidget *parent = nullptr);
    void refreshHistory(); 

private slots:
    void onAddTransaction();
    void updateAccountSelector(); // Слот оновлення рахунків
    void refreshPlannedList();

private:
    // --- Quick Add UI ---
    QComboBox *typeSelector;    // Expense/Income
    QDoubleSpinBox *amountInput;
    QComboBox *accountSelector; // Вибір рахунку (замість категорії)
    QLineEdit *noteInput;
    
    // --- History UI ---
    QVBoxLayout *historyLayout; 

    // --- Planned Section UI ---
    QWidget *plannedContainer; 
    QVBoxLayout *plannedListLayout;
    
    QDateEdit *planDateInput;
    QDoubleSpinBox *planAmountInput;
    QLineEdit *planTitleInput;

    // --- Setup Methods ---
    void setupQuickAdd(QVBoxLayout *parentLayout);
    void setupHistory(QVBoxLayout *parentLayout);
    void setupPlannedSection(QVBoxLayout *parentLayout);
};

#endif // FINANCEWORKSPACE_H