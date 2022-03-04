#ifndef COMMANDMODEWINDOW_H
#define COMMANDMODEWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QShowEvent>
#include <QCloseEvent>
#include <QStringBuilder>
#include <QAbstractItemView>

#include "communication.h"
#include "closedialog.h"
#include "newfiledialog.h"

namespace Ui
{
class CommandModeWindow;
}

class CommandModeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CommandModeWindow(QWidget *parent = nullptr, int serverDescriptor = 0);
    ~CommandModeWindow();

    void setDescriptor(int descriptor);
    void setLoginWindow(QMainWindow* loginWindow);
    void setEditWindow(QMainWindow* editWindow);

    void updateFileExplorer(deque<QString> & files);

private:
    Ui::CommandModeWindow *ui;

    QMainWindow * loginW = nullptr;
    QMainWindow * editW = nullptr;

    int serverDescriptor;

    bool logout = false;

    NewFileDialog nfd;

private slots:

    void showEvent(QShowEvent * event);
    void closeEvent (QCloseEvent *event);

    void on_openButton_clicked();

    void on_createButton_clicked();

    void on_downloadButton_clicked();

    void on_deleteButton_clicked();

    void on_quitButton_clicked();

    void on_logoutButton_clicked();

    void on_permButton_clicked();

};

#endif // COMMANDMODEWINDOW_H
