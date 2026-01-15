#include "ui/MainWindow.h" 
#include "core/StorageManager.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    StorageManager::instance().init();
    
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