#ifndef LOADCOLLECTIONDIALOG_H
#define LOADCOLLECTIONDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QList>
#include <QUrl>

namespace Ui {
class LoadCollectionDialog;
}

class LoadCollectionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LoadCollectionDialog(QWidget *parent = 0);
    ~LoadCollectionDialog();

    void setSettingsOptions(QSettings::Format f, QSettings::Scope s, QString org, QString app);
    void createCollection();

    QList<QUrl> getSelectedCollection(QString &col_name);

private slots:
    void on_listWidget_doubleClicked(const QModelIndex &index);

    void on_pushButton_deleteColl_clicked();

private:
    Ui::LoadCollectionDialog *ui;

    QSettings::Format m_qSet_format;
    QSettings::Scope m_qSet_scope;
    QString m_qSet_organization;
    QString m_qSet_application;
};

#endif // LOADCOLLECTIONDIALOG_H
