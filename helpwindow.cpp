#include "helpwindow.h"
#include "ui_helpwindow.h"

HelpWindow::HelpWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpWindow)
{
    ui->setupUi(this);

    this->setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint );
    this->setLanguage(GERMAN);
}

HelpWindow::~HelpWindow()
{
    delete ui;
}

void HelpWindow::setLanguage(Language lang)
{
    switch (lang) {
    case GERMAN:
        ui->textBrowser->setSource(QUrl("qrc:///doc/help_de.html"));
        break;
    case ENGLISH:
        ui->textBrowser->setSource(QUrl("qrc:///doc/help_en.html"));
        break;
    default:
        break;
    }
}
