#ifndef EDITMODEWINDOW_H
#define EDITMODEWINDOW_H

#include <QDialog>
#include <QShowEvent>
#include <QCloseEvent>
#include <QMessageBox>


#include "communication.h"
#include "operation.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

namespace Ui
{
class EditModeWindow;
}

class EditModeWindow : public QDialog
{
    Q_OBJECT

public:
    explicit EditModeWindow(QWidget *parent = nullptr, int serverDescriptorParam = 0);
    ~EditModeWindow();



private slots:
    void showEvent(QShowEvent *event);
    void closeEvent (QCloseEvent *event);

    void textFieldChanged(int position, int removed, int inserted);



private:
    Operation op;
    Communication commObj;


    static unsigned int tooFastCounter;
    static bool isUpdateRunning;
    static bool isTextChanged;
    static int serverDescriptor;
    static Ui::EditModeWindow *ui;


    static void* update(void* arg);
    static void processServerResponse(string fromServer);
    static void applyOperation(Operation *op);

    pthread_t tid;



};

#endif // EDITMODEWINDOW_H
