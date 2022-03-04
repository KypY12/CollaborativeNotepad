#include "closedialog.h"
#include "ui_closedialog.h"

CloseDialog::CloseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CloseDialog)
{
    ui->setupUi(this);
    QDialog::setWindowTitle ( "Verificare" );
}

CloseDialog::~CloseDialog()
{
    delete ui;
}

bool CloseDialog::getCloseResult()
{
    return close;
}

void CloseDialog::on_buttonBox_accepted()
{
    close = true;
}

void CloseDialog::on_buttonBox_rejected()
{
    close = false;
}
