#include "ui/MainWindow.h" // Зверни увагу на шлях до хедера
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    QFile file(":/style.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        // Застосовуємо стиль до ВСІЄЇ програми одразу
        a.setStyleSheet(stream.readAll());
        file.close();
    }

    MainWindow w;
    w.show();
    return a.exec();
    
}