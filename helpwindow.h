#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QDialog>
#include <QFile>

#include "global.h"

namespace Ui {
class HelpWindow;
}

class HelpWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit HelpWindow(QWidget *parent = 0);
    ~HelpWindow();

    void setLanguage(Language lang);
    
private:
    Ui::HelpWindow *ui;
};

#endif // HELPWINDOW_H
