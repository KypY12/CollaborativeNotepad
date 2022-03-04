#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QShowEvent>
#include <QCloseEvent>

#include "communication.h"
#include "closedialog.h"


namespace Ui
{
class LoginWindow;
}

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr, int serverDescriptor = 0);
    ~LoginWindow();


    void setCmdWindow(QMainWindow * cmdWindow);

private:
    Ui::LoginWindow *ui;

    QMainWindow *cmdW = nullptr;

    int serverDescriptor;

    bool loggedIn = false;

    bool checkLogin();
    bool checkRegister();

private slots:

    void showEvent(QShowEvent *event);
    void closeEvent (QCloseEvent *event);

    void on_loginButton_clicked();

    void on_registerButton_clicked();


};

#endif // LOGINWINDOW_H
