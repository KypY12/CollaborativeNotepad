#include "editmodewindow.h"
#include "ui_editmodewindow.h"


unsigned int EditModeWindow::tooFastCounter = 0;
bool EditModeWindow::isUpdateRunning = false;
bool EditModeWindow::isTextChanged = false;
int EditModeWindow::serverDescriptor = 0;
Ui::EditModeWindow * EditModeWindow::ui = new Ui::EditModeWindow;


EditModeWindow::EditModeWindow(QWidget *parent, int serverDescriptorParam) :
    QDialog(parent)
{
    EditModeWindow::serverDescriptor = serverDescriptorParam;
    ui->setupUi(this);

    connect( ui->textField->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(textFieldChanged(int,int,int)));

}

EditModeWindow::~EditModeWindow()
{
    delete ui;
}

void EditModeWindow::showEvent(QShowEvent *event)
{
    int error;
    error = pthread_create(&(tid), nullptr, &update, nullptr);
    if (error != 0)
        printf("\nThread can't be created : [%s]", strerror(error));



//if(unBlockServerDesc())
//return;


//shortTimer->start(1000);

    event->accept();

}



void EditModeWindow::closeEvent(QCloseEvent *event)
{
    while(isUpdateRunning);

    //if(blockServerDesc())
    //return;

    string empty;
    Operation opExit(EXIT,0,0,empty);
    commObj.sendOperationToDescriptor(opExit);

    //Operation * exitTest = commObj.recvOperationFromDescriptor();

    //cout << "|" << exitTest->getType() << "|" << endl;

    //  if(exitTest->getType() == EXIT)
//        {


    //this->hide();
    //cmdW->show();
    //event->accept();
//        }
//    else
//        {
//            shortTimer->start(100);
    event->accept();
//        }


}

void EditModeWindow::textFieldChanged(int position, int removed, int inserted)
{
    isTextChanged = true;
    while(isUpdateRunning);


    if(removed == 0 && inserted > 0)        // Operatie de inserare
        {
            string textInserted = ui->textField->toPlainText().mid(position,inserted).toStdString();
            Operation tempOp(INSERT, inserted, position, textInserted);

            op = tempOp;
            commObj.sendOperationToDescriptor(op);

            //cout << "AM INTRODUS" <<endl;
        }
    else if (removed > 0 && inserted == 0)  // Operatie de stergere
        {
            ui->textField->undo();
            string textRemoved = ui->textField->toPlainText().mid(position, removed).toStdString();
            ui->textField->redo();

            Operation tempOp(REMOVE, removed, position, textRemoved);

            op = tempOp;
            commObj.sendOperationToDescriptor(op);

        }
    else if(removed > 0 && inserted > 0)    // Operatie de stergere urmata de inserare (atunci cand se inlocuieste o selectie de text cu alt text)
        {
//            ui->textField->undo();
//            string textRemoved = ui->textField->toPlainText().mid(position, removed).toStdString();
//            ui->textField->redo();
            string textInserted = ui->textField->toPlainText().mid(position,inserted).toStdString();

            //Operation tempOp(MIXED, removed, position, textRemoved, textInserted);
            Operation tempOp(MIXED, removed, position, textInserted);

            op = tempOp;
            commObj.sendOperationToDescriptor(op);

        }
    //cout << endl << "OPERATIE" << endl;

    ++tooFastCounter;
    isTextChanged = false;
}

void *EditModeWindow::update(void *arg)
{
    Communication commObj(serverDescriptor);

    while(1)
        {
            //cout << "AM VENIT AICI LA VOI" << endl;
            string serverUpdate = "maine";//commObj.recvStringFromDescriptor();

            // cout << "|" << serverUpdate << "|" << endl;
            //cout << "sasas" << endl;
            if(serverUpdate.substr(0,6) == "update")
                {
                    cout << "Test" << endl;
                    isUpdateRunning = true;
                    while(isTextChanged);

                    cout<< "AICI AM AJUNS" << endl;
//                    if(blockServerDesc())
//                        return nullptr;

                    //processServerResponse(commObj.recvStringFromDescriptor());

//                    if(unBlockServerDesc())
//                        return nullptr;
                    isUpdateRunning = false;
                }
            else if(serverUpdate.substr(0,4) == "exit")
                {
                    break;
                }


        }

    return nullptr;
}

void EditModeWindow::processServerResponse(string fromServer)
{
    Communication commObj(serverDescriptor);
    if(fromServer.substr(0,5) == "peace")
        {
            Operation * tempOp = commObj.recvOperationFromDescriptor();
            if(tempOp->getType() == DEFAULT)
                {
                    return;
                }

            applyOperation(tempOp);
        }
    else if(fromServer.substr(0,8) == "conflict")
        {
            Operation * tempOp1 = commObj.recvOperationFromDescriptor();
            Operation * tempOp2 = commObj.recvOperationFromDescriptor();

            for(unsigned int opIndex = 0; opIndex<tooFastCounter; opIndex++)
                {
                    ui->textField->undo();
                }

            applyOperation(tempOp1);
            applyOperation(tempOp2);

        }

    tooFastCounter = 0;
}


void EditModeWindow::applyOperation(Operation *op)
{
    TypeOfOp typeOp = op->getType();
    QString textToUse;
    QTextCursor tempCurs = ui->textField->textCursor();

    switch(typeOp)
        {
        case INSERT:
            textToUse = op->getCharsInserted().c_str();
            tempCurs.setPosition(op->getPosition());
            ui->textField->insertPlainText(textToUse);
            //ui->textField->setTextCursor(tempCurs);

            break;

        case REMOVE:
            tempCurs.setPosition(op->getPosition());
            tempCurs.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor, op->getNoChars());
            tempCurs.removeSelectedText();
            //ui->textField->setTextCursor(tempCurs);


            break;

        case MIXED:
            tempCurs.setPosition(op->getPosition());
            tempCurs.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor, op->getNoChars());
            tempCurs.removeSelectedText();

            textToUse = op->getCharsInserted().c_str();
            ui->textField->insertPlainText(textToUse);

            break;

        default:
            return;
        }

}
