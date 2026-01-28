#ifndef CHARTPANEL_H
#define CHARTPANEL_H

#include <QWidget>

class ChartPanel : public QWidget {
    Q_OBJECT
public:
    explicit ChartPanel(QWidget *parent = nullptr);
};

#endif // CHARTPANEL_H