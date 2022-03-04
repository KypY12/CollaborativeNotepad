#ifndef EDITMODE_H
#define EDITMODE_H

#include <QMainWindow>
#include <QShowEvent>
#include <QCloseEvent>
#include <QTimer>
#include <QMessageBox>

#include "communication.h"
#include "operation.h"
#include "closedialog.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


namespace Ui
{
class EditMode;
}

class EditMode : public QMainWindow
{
    Q_OBJECT

public:
    pthread_mutex_t lock;

    explicit EditMode(QWidget *parent = nullptr, int serverDescriptor = 0);
    ~EditMode();
    void setDescriptor(int serverDescriptorParam);

    Operation* getOperation();
    void setOperation(Operation &opParam);
    void setCmdWindow(QMainWindow * cmdWindow);

private slots:
    void showEvent(QShowEvent *event);
    void closeEvent (QCloseEvent *event);

    void textFieldChanged(int position, int removed, int inserted);

    void updateProcessing();

private:
    Ui::EditMode *ui;
    QTimer timer;

    Operation op;
    QMainWindow * cmdW = nullptr;



    void processServerResponse(string fromServer);
    void applyOperation(Operation *op);


    static void* createThread(void *args);
    void* update(void);

    pthread_t tid;

};

#endif // EDITMODE_H
