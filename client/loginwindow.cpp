#include "loginwindow.h"
#include "ui_loginwindow.h"

bool logInNow;


LoginWindow::LoginWindow(QWidget *parent, int serverDescriptor) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow),
    serverDescriptor(serverDescriptor)
{
    ui->setupUi(this);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::setCmdWindow(QMainWindow *cmdWindow)
{
    cmdW = cmdWindow;
}


void LoginWindow::closeEvent (QCloseEvent *event)
{

    if(!loggedIn)
        {
            CloseDialog checkClose;
            checkClose.show();
            checkClose.exec();

            if (checkClose.getCloseResult())
                {
                    Communication commObj(serverDescriptor);
                    commObj.sendStringToDescriptor("quit");

                    if(commObj.recvStringFromDescriptor().substr(0,4) == "quit")
                        {
                            event->accept();
                        }
                    else
                        {
                            event->ignore();
                        }
                }
            else
                {
                    event->ignore();
                }

        }
    else
        {
            this->hide();
            cmdW->show();
            event->ignore();
        }


}


void LoginWindow::showEvent(QShowEvent *event)
{
    loggedIn = false;
    event->accept();
}


bool LoginWindow::checkLogin()
{

    Communication commObj(serverDescriptor);

    commObj.sendStringToDescriptor("login");
    commObj.sendStringToDescriptor(ui->username->text().toUtf8().constData()); // trimitem username-ul
    commObj.sendStringToDescriptor(ui->password->text().toUtf8().constData()); // trimitem parola

    string test = commObj.recvStringFromDescriptor();

    if(test.substr(0,7) == "SUCCESS")
        {
            return true;
        }
    else
        {
            return false;
        }

}


bool LoginWindow::checkRegister()
{
    Communication commObj(serverDescriptor);

    commObj.sendStringToDescriptor("register");
    commObj.sendStringToDescriptor(ui->username->text().toUtf8().constData()); // trimitem username-ul
    commObj.sendStringToDescriptor(ui->password->text().toUtf8().constData()); // trimitem parola

    string test = commObj.recvStringFromDescriptor();


    if(test.substr(0,7) == "SUCCESS")
        {
            return true;
        }
    else
        {
            return false;
        }
}


void LoginWindow::on_loginButton_clicked()
{
    if(checkLogin())
        {
            loggedIn = true;
            logInNow = true;
            this->close();

        }
    else
        {
            QString infoMessage("Date de autentificare invalide!");
            ui->infoLabel->setText(infoMessage);
        }

}


void LoginWindow::on_registerButton_clicked()
{
    if(checkRegister())
        {
            QString infoMessage("Înregistrare cu succes!");
            ui->infoLabel->setText(infoMessage);
        }
    else
        {
            QString infoMessage("Acest utilizator exista deja!");
            ui->infoLabel->setText(infoMessage);
        }

}






