#include "newfiledialog.h"
#include "ui_newfiledialog.h"
#include <iostream>
using namespace std;

NewFileDialog::NewFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFileDialog)
{
    ui->setupUi(this);
    QDialog::setWindowTitle ( "Fereastră de intrare" );
}

NewFileDialog::~NewFileDialog()
{
    delete ui;
}

void NewFileDialog::setLabelText(string labelText)
{
    QString labelTxt(labelText.c_str());
    ui->labelInfo->setText(labelTxt);
}

QString NewFileDialog::getFileName()
{

    return fileNameStr;

}

void NewFileDialog::clearString()
{
    fileNameStr.clear();
}


void NewFileDialog::on_okCancelButton_accepted()
{
    fileNameStr = ui->fileName->text();
}



void NewFileDialog::on_okCancelButton_rejected()
{
    fileNameStr = "";
}
