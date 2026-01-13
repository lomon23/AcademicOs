#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout> // Підключаємо сітку
#include <QPushButton> // Для тесту кнопок

class MainWindow : public QMainWindow{
        Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private:
        // Замість Ui::MainWindow *ui; ми створюємо вказівники на наші віджети тут
        QWidget *centralWidget; // Головний контейнер
        QGridLayout *mainLayout; // Наша Bento-сітка
};
#endif // MAINWINDOW_H