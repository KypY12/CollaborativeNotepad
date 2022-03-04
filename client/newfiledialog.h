#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include <QDialog>
#include <iostream>
#include <string>

using namespace std;

namespace Ui
{
class NewFileDialog;
}

class NewFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewFileDialog(QWidget *parent = nullptr);
    ~NewFileDialog();

    void setLabelText(string labelText);
    QString getFileName();
    void clearString();

private slots:

    void on_okCancelButton_accepted();

    void on_okCancelButton_rejected();

private:
    Ui::NewFileDialog *ui;
    QString fileNameStr;

};

#endif // NEWFILEDIALOG_H
