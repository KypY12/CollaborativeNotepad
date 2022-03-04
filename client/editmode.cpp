#include "editmode.h"
#include "ui_editmode.h"

unsigned int tooFastCounter = 0;
bool isUpdateRunning = false;
bool isTextChanged = false;
bool isEditOpen = false;
bool isOpProcessed = false;
int serverDescriptor = 0;

bool isApplyOn = false;

string fromServer = "";

Operation globalOp1;
Operation globalOp2;


void*  EditMode::createThread(void *args)
{
    return ((EditMode*)args)->update();
}

EditMode::EditMode(QWidget *parent, int serverDescriptorParam) :
    QMainWindow(parent),
    ui(new Ui::EditMode)
{
    ui->setupUi(this);
    serverDescriptor = serverDescriptorParam;

    connect(ui->textField->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(textFieldChanged(int,int,int)));

    connect(&timer, SIGNAL(timeout()), this, SLOT(updateProcessing()));
    timer.start();

}


EditMode::~EditMode()
{
    timer.stop();
    delete ui;
}


Operation* EditMode::getOperation()
{
    return &op;
}

void EditMode::setOperation(Operation& opParam)
{
    op = opParam;
}

void EditMode::setCmdWindow(QMainWindow *cmdWindow)
{
    cmdW = cmdWindow;
}


void EditMode::setDescriptor(int descriptor)
{
    serverDescriptor = descriptor;

}

void EditMode::showEvent(QShowEvent *event)
{
    Communication commObj(serverDescriptor);
    isApplyOn = true;
    ui->textField->clear();

    string fileContent = commObj.recvStringFromDescriptor();

    ui->textField->setText(fileContent.c_str());
    isApplyOn = false;

    isEditOpen = true;

    int error;

    if ((error = pthread_mutex_init(&lock, nullptr)) != 0)
        {
            perror("[SERVER] | [ERROR] : MUTEX INIT HAS FAILED :\n");
            perror(strerror(error));
        }


    error = pthread_create(&(tid), nullptr, &createThread, this);
    if (error != 0)
        {
            perror("[CLIENT] | [ERROR] : THREAD CAN'T BE CREATED :\n");
            perror(strerror(error));
        }




    event->accept();


}


void EditMode::closeEvent(QCloseEvent *event)
{
    pthread_mutex_lock(&lock);

    while(isUpdateRunning)
        {
            pthread_mutex_unlock(&lock);
            usleep(1000);
            pthread_mutex_lock(&lock);
        }

    isEditOpen = false;
    pthread_mutex_unlock(&lock);


    CloseDialog checkClose;
    checkClose.show();
    checkClose.exec();

    if (checkClose.getCloseResult())
        {

            string empty;
            Operation opExit(EXIT,0,0,empty);
            pthread_mutex_lock(&lock);
            Communication commObj(serverDescriptor);
            pthread_mutex_unlock(&lock);
            commObj.sendOperationToDescriptor(opExit);


            pthread_mutex_lock(&lock);
            if(!isEditOpen)
                {
                    this->hide();
                    cmdW->show();
                }
            pthread_mutex_unlock(&lock);

            event->ignore();

        }
    else
        {
            pthread_mutex_lock(&lock);
            isEditOpen = true;
            pthread_mutex_unlock(&lock);

            event->ignore();
        }


}



void EditMode::textFieldChanged(int position, int removed, int inserted)
{

    if(isApplyOn)
        return;

    pthread_mutex_lock(&lock);
    while(isUpdateRunning)
        {
            pthread_mutex_unlock(&lock);
            usleep(1000);
            pthread_mutex_lock(&lock);
        }
    if(isEditOpen)
        {
            while(isUpdateRunning)
                {
                    pthread_mutex_unlock(&lock);
                    usleep(1000);
                    pthread_mutex_lock(&lock);
                }

            Communication commObj(serverDescriptor);

            if(removed == 0 && inserted > 0)        // Operatie de inserare
                {
                    string textInserted = ui->textField->toPlainText().mid(position,inserted).toStdString();
                    Operation tempOp(INSERT, inserted, position, textInserted);

                    commObj.sendOperationToDescriptor(tempOp);

                }
            else if (removed > 0 && inserted == 0)  // Operatie de stergere
                {
                    ui->textField->undo();
                    string textRemoved = ui->textField->toPlainText().mid(position, removed).toStdString();
                    ui->textField->redo();

                    Operation tempOp(REMOVE, removed, position, textRemoved);

                    commObj.sendOperationToDescriptor(tempOp);


                }
            else if(removed > 0 && inserted > 0)    // Operatie de stergere urmata de inserare (atunci cand se inlocuieste o selectie de text cu alt text)
                {

                    string textInserted = ui->textField->toPlainText().mid(position,inserted).toStdString();
                    Operation tempOp(MIXED, removed, position, textInserted);

                    commObj.sendOperationToDescriptor(tempOp);


                }

            ++tooFastCounter;
            isTextChanged = false;

        }
    pthread_mutex_unlock(&lock);

}

void EditMode::updateProcessing()
{
    pthread_mutex_lock(&lock);
    if(isOpProcessed)
        {
            processServerResponse(fromServer);
            isOpProcessed = false;
        }
    pthread_mutex_unlock(&lock);
}



void* EditMode::update(void)
{
    pthread_detach(pthread_self());

    pthread_mutex_lock(&lock);
    Communication comObj(serverDescriptor);
    pthread_mutex_unlock(&lock);

    string serverUpdate;
    while(1)
        {
            serverUpdate = comObj.recvStringFromDescriptor();

            if(serverUpdate == "update")
                {
                    pthread_mutex_lock(&lock);
                    isUpdateRunning = true;
                    pthread_mutex_unlock(&lock);

                    serverUpdate = comObj.recvStringFromDescriptor();

                    pthread_mutex_lock(&lock);
                    fromServer = serverUpdate;
                    pthread_mutex_unlock(&lock);

                    pthread_mutex_lock(&lock);
                    if(fromServer == "peace")
                        {
                            globalOp1 = *(comObj.recvOperationFromDescriptor());
                            if(globalOp1.getType() == DEFAULT)
                                {
                                    continue;
                                }

                        }
                    else if(fromServer == "conflict")
                        {
                            globalOp1 = *(comObj.recvOperationFromDescriptor());
                            globalOp2 = *(comObj.recvOperationFromDescriptor());

                        }

                    isOpProcessed = true;
                    while(isOpProcessed)
                        {
                            pthread_mutex_unlock(&lock);
                            usleep(1000);
                            pthread_mutex_lock(&lock);
                        }

                    isUpdateRunning = false;
                    pthread_mutex_unlock(&lock);
                }
            else if(serverUpdate == "exit")
                {
                    pthread_mutex_lock(&lock);
                    isEditOpen = false;
                    pthread_mutex_unlock(&lock);
                    break;
                }

        }

    pthread_exit(nullptr);

}

void EditMode::processServerResponse(string fromServer)
{
    Communication commObj(serverDescriptor);

    if(fromServer.substr(0,5) == "peace")
        {
            if(globalOp1.getType() == DEFAULT)
                {
                    return;
                }

            isApplyOn = true;
            tooFastCounter = 0;
            applyOperation(&globalOp1);
            isApplyOn = false;
        }
    else if(fromServer.substr(0,8) == "conflict")
        {
            isApplyOn = true;
            for(unsigned int opIndex = 0; opIndex<tooFastCounter; opIndex++)
                {
                    ui->textField->undo();
                }
            applyOperation(&globalOp1);
            applyOperation(&globalOp2);
            tooFastCounter = 0;
            isApplyOn = false;
        }


}


void EditMode::applyOperation(Operation *op)
{
    TypeOfOp typeOp = op->getType();
    QString textToUse;
    QTextCursor tempCurs = ui->textField->textCursor();
    QTextCursor prevCurs = ui->textField->textCursor();

    switch(typeOp)
        {
        case INSERT:

            textToUse = op->getCharsInserted().c_str();

            tempCurs.setPosition(op->getPosition());
            ui->textField->setTextCursor(tempCurs);
            ui->textField->insertPlainText(textToUse);
            ui->textField->setTextCursor(prevCurs);

            break;

        case REMOVE:
            tempCurs.setPosition(op->getPosition());
            tempCurs.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor, op->getNoChars());
            ui->textField->setTextCursor(tempCurs);
            tempCurs.removeSelectedText();
            ui->textField->setTextCursor(prevCurs);

            break;

        case MIXED:
            tempCurs.setPosition(op->getPosition());
            tempCurs.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor, op->getNoChars());
            tempCurs.removeSelectedText();

            textToUse = op->getCharsInserted().c_str();
            ui->textField->setTextCursor(tempCurs);
            ui->textField->insertPlainText(textToUse);
            ui->textField->setTextCursor(prevCurs);

            break;

        default:
            return;
        }

}







