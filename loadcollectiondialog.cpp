#include "loadcollectiondialog.h"
#include "ui_loadcollectiondialog.h"

LoadCollectionDialog::LoadCollectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadCollectionDialog)
{
    ui->setupUi(this);
    this->setModal(true);
    this->setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint );

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->pushButton_deleteColl->setEnabled(false);

    this->m_qSet_organization = "bsSoft";
    this->m_qSet_application = "picture-show2";
}

LoadCollectionDialog::~LoadCollectionDialog()
{
    delete ui;
}

void LoadCollectionDialog::setSettingsOptions(QSettings::Format f, QSettings::Scope s, QString org, QString app)
{
    this->m_qSet_application = app;
    this->m_qSet_organization = org;
    this->m_qSet_format = f;
    this->m_qSet_scope = s;
}

void LoadCollectionDialog::createCollection()
{
    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
    QStringList groups = settings.childGroups();
    foreach (QString gr, groups)
    {
        QString col_name = gr.section("_", 1);
        settings.beginGroup(gr);
        int num_images = settings.value("num_images", QVariant(0)).toInt();
        int num_folders = settings.value("num_folders", QVariant(0)).toInt();
        settings.endGroup();
        QIcon imgIcon;
        if (num_folders > 0)
            imgIcon.addFile(QStringLiteral(":/img/picturesFolder-16.png"), QSize(), QIcon::Normal, QIcon::Off);
        else
            imgIcon.addFile(QStringLiteral(":/img/images-16.png"), QSize(), QIcon::Normal, QIcon::Off);
        QListWidgetItem * newListItem = new QListWidgetItem(imgIcon, tr("%1 (Smart Folders: %2, Bilder: %3)").arg(col_name).arg(num_folders).arg(num_images));
        ui->listWidget->addItem(newListItem);
    }

    if (ui->listWidget->count() > 0)
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        ui->pushButton_deleteColl->setEnabled(true);
    }
    else
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        ui->pushButton_deleteColl->setEnabled(false);
    }

    ui->listWidget->setCurrentRow(0);
}

QList<QUrl> LoadCollectionDialog::getSelectedCollection(QString & col_name)
{
    int selected_row = ui->listWidget->currentRow();

    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
    int c = 0;
    QStringList groups = settings.childGroups();
    foreach (QString gr, groups)
    {
        if (c == selected_row)
        {
            settings.beginGroup(gr);
            QStringList url_list = settings.value("url_list", QVariant(QStringList())).toStringList();
            settings.endGroup();

            col_name = gr.section("_", 1);

            QList<QUrl> collection_list;

            foreach(QString url, url_list)
                collection_list.append(QUrl(url));

            return collection_list;
        }

        c++;
    }
}

void LoadCollectionDialog::on_listWidget_doubleClicked(const QModelIndex &index)
{
     this->accept();
}

void LoadCollectionDialog::on_pushButton_deleteColl_clicked()
{
    int selected_row = ui->listWidget->currentRow();

    QSettings settings(m_qSet_format, m_qSet_scope, m_qSet_organization, m_qSet_application);
    int c = 0;
    QStringList groups = settings.childGroups();
    foreach (QString gr, groups)
    {
        if (c == selected_row)
        {
            settings.beginGroup(gr);
            settings.remove("");
            settings.endGroup();

            ui->listWidget->clear();
            this->createCollection();

            return;
        }

        c++;
    }
}
