#include "ui/MainWindow.h" // Зверни увагу на шлях до хедера
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}