#include "commandmodewindow.h"
#include "ui_commandmodewindow.h"

extern bool logInNow;

extern int errno;

CommandModeWindow::CommandModeWindow(QWidget *parent, int serverDescriptor) :
    QMainWindow(parent),
    ui(new Ui::CommandModeWindow),
    serverDescriptor(serverDescriptor)
{
    ui->setupUi(this);
    ui->fileExplorer->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

CommandModeWindow::~CommandModeWindow()
{
    delete ui;

}


void CommandModeWindow::closeEvent (QCloseEvent *event)
{

    CloseDialog checkClose;
    checkClose.show();
    checkClose.exec();

    if (checkClose.getCloseResult())
        {
            Communication commObj(serverDescriptor);
            commObj.sendStringToDescriptor("quit");
            string confirmation = commObj.recvStringFromDescriptor();

            if(confirmation.substr(0,4) == "quit")
                {
                    ui->labelInfo->clear();
                    ui->fileExplorer->clear();
                    event->accept();
                }
            else
                {
                    QString infoMessage("Serverul nu a putut prelua comanda de ieșire!");
                    ui->labelInfo->setText(infoMessage);
                    event->ignore();
                }

        }
    else
        {
            event->ignore();
        }


}


void CommandModeWindow::showEvent(QShowEvent *event)
{
    logout = false;

    if(logInNow)
        {

            Communication commObj(serverDescriptor);

            deque<QString> files;
            string messageRecv = commObj.recvStringFromDescriptor();

            while(messageRecv != "ALL FILES SENT!")
                {
                    files.push_back(messageRecv.c_str());
                    messageRecv = commObj.recvStringFromDescriptor();
                }

            updateFileExplorer(files);

            logInNow = false;
        }

    event->accept();
}


void CommandModeWindow::setDescriptor(int descriptor)
{
    serverDescriptor = descriptor;
}

void CommandModeWindow::setLoginWindow(QMainWindow *loginWindow)
{
    loginW = loginWindow;
}

void CommandModeWindow::setEditWindow(QMainWindow *editWindow)
{
    editW = editWindow;
}

void CommandModeWindow::updateFileExplorer(deque<QString> &files)
{
    ui->fileExplorer->clear();
    for(auto it : files)
        {
            ui->fileExplorer->addItem(it);
        }
}


void CommandModeWindow::on_createButton_clicked()
{
    ui->labelInfo->clear();
    if(this->isVisible())
        {

            Communication commObj(serverDescriptor);

            commObj.sendStringToDescriptor("create");

            string confirmation = commObj.recvStringFromDescriptor();


            if(confirmation.substr(0,5) == "Ready")
                {
                    QString fileName;

                    bool cancelPressed = false;

                    nfd.setLabelText("");
                    while(1)
                        {

                            nfd.clearString();
                            nfd.show();
                            nfd.exec();

                            fileName = nfd.getFileName();

                            if(fileName.length() == 0)
                                {
                                    cancelPressed = true;
                                    break;
                                }

                            commObj.sendStringToDescriptor(fileName.toStdString());
                            confirmation = commObj.recvStringFromDescriptor();

                            if(confirmation.substr(0,5) == "Done!")
                                {
                                    ui->fileExplorer->addItem(fileName);
                                    break;
                                }
                            else if(confirmation.substr(0,6) == "Exists")
                                {
                                    nfd.setLabelText("Acest fișier există deja!");
                                }
                            else
                                {
                                    QString infoMessage("A apărut o eroare în timpul creării! Încercați din nou!");
                                    ui->labelInfo->setText(infoMessage);
                                    break;
                                }

                        }

                    if(cancelPressed)
                        {
                            commObj.sendStringToDescriptor("");
                        }


                }
            else
                {

                    QString infoMessage("Serverul nu a putut prelua comanda!");
                    ui->labelInfo->setText(infoMessage);
                }

        }

}


void CommandModeWindow::on_openButton_clicked()
{
    ui->labelInfo->clear();
    if(this->isVisible())
        {
            int selectedItems = ui->fileExplorer->selectedItems().size();
            if(selectedItems == 1)
                {

                    Communication commObj(serverDescriptor);

                    commObj.sendStringToDescriptor("open");

                    QString fileName = ui->fileExplorer->currentItem()->text();

                    commObj.sendStringToDescriptor(fileName.toStdString());


                    string confirmation = commObj.recvStringFromDescriptor();

                    if(confirmation == "FULL")
                        {
                            QString infoMessage("Fișierul este deja editat de două persoane!");
                            ui->labelInfo->setText(infoMessage);
                        }
                    else if(confirmation == "SUCCESS")
                        {

                            editW->show();
                            this->hide();
                        }
                    else
                        {
                            QString infoMessage("Fișierul nu a putut fi deschis!");
                            ui->labelInfo->setText(infoMessage);
                        }


                }
            else if (selectedItems == 0)
                {

                    QString infoMessage("Niciun fișier selectat!");
                    ui->labelInfo->setText(infoMessage);

                }
            else if (selectedItems > 1)
                {
                    QString infoMessage("Vă rugăm să selectați doar un fișier pentru deschidere!");
                    ui->labelInfo->setText(infoMessage);
                }
        }

}


void CommandModeWindow::on_downloadButton_clicked()
{
    ui->labelInfo->clear();

    if(this->isVisible())
        {

            QList<QListWidgetItem*> selectedItems = ui->fileExplorer->selectedItems();

            if(selectedItems.size() == 0)
                {
                    QString infoMessage("Vă rugăm să selectați măcar un fișier pentru descărcare!");
                    ui->labelInfo->setText(infoMessage);
                }
            else
                {
                    Communication commObj(serverDescriptor);

                    commObj.sendStringToDescriptor("download");

                    string confirmation = commObj.recvStringFromDescriptor();

                    if(confirmation.substr(0,5) == "Ready")
                        {
                            QString noFiles = QString::number(selectedItems.size());
                            commObj.sendStringToDescriptor(noFiles.toStdString());

                            bool error = false;
                            QString fileName;

                            // Selecteaza calea pentru descarcare
                            //QFileDialog downloadLocation;
                            QString dirName = QFileDialog::getExistingDirectory(this, tr("Download to specified directory"), "~/", QFileDialog::DontUseNativeDialog);

                            //downloadLocation.exec();
                            QString currentPath = QDir::currentPath();
                            QDir::setCurrent(dirName);

                            foreach(QListWidgetItem * item, selectedItems)
                                {
                                    fileName = item->text();
                                    commObj.sendStringToDescriptor(fileName.toStdString());

                                    confirmation = commObj.recvStringFromDescriptor();

                                    if(confirmation.substr(0,5) == "Done!")
                                        {

                                            // Descarca fiecare fisier
                                            QString fileContent = commObj.recvStringFromDescriptor().c_str();

                                            if(fileContent.isEmpty())
                                                {
                                                    commObj.recvStringFromDescriptor();
                                                }

                                            int copyNumber = 1;
                                            QString fileCheck = fileName;
                                            while(QFile::exists(fileCheck))
                                                {
                                                    fileCheck = fileName + " Copy(" + std::to_string(copyNumber++).c_str() + ")";
                                                }
                                            fileName = fileCheck;

                                            QFile fileDownloded(fileName);
                                            if(!fileDownloded.open(QFile::WriteOnly | QFile::Text))
                                                {
                                                    error  = true;
                                                }
                                            else
                                                {
                                                    QTextStream outputStream(&fileDownloded);
                                                    outputStream << fileContent;
                                                    fileDownloded.close();
                                                }

                                        }
                                    else
                                        {
                                            error = true;
                                        }

                                }

                            if(error)
                                {
                                    QString infoMessage("Nu s-au putut descărca toate fișierele selectate!");
                                    ui->labelInfo->setText(infoMessage);
                                }

                            QDir::setCurrent(currentPath);
                        }
                    else
                        {
                            QString infoMessage("Serverul nu a putut prelua comanda!");
                            ui->labelInfo->setText(infoMessage);
                        }


                }

        }

}


void CommandModeWindow::on_deleteButton_clicked()
{
    ui->labelInfo->clear();
    if(this->isVisible())
        {
            QList<QListWidgetItem*> selectedItems = ui->fileExplorer->selectedItems();

            if(selectedItems.size() == 0)
                {
                    QString infoMessage("Vă rugăm să selectați măcar un fișier pentru ștergere!");
                    ui->labelInfo->setText(infoMessage);
                }
            else
                {
                    Communication commObj(serverDescriptor);

                    commObj.sendStringToDescriptor("delete");

                    string confirmation = commObj.recvStringFromDescriptor();

                    if(confirmation.substr(0,5) == "Ready")
                        {
                            QString noFiles = QString::number(selectedItems.size());
                            commObj.sendStringToDescriptor(noFiles.toStdString());

                            bool error = false;
                            QString fileName;

                            foreach(QListWidgetItem * item, selectedItems)
                                {
                                    fileName = item->text();
                                    commObj.sendStringToDescriptor(fileName.toStdString());

                                    confirmation = commObj.recvStringFromDescriptor();

                                    if(confirmation.substr(0,5) == "Done!")
                                        {
                                            ui->fileExplorer->removeItemWidget(item);
                                            delete item;
                                        }
                                    else
                                        {
                                            error = true;
                                        }

                                }

                            if(error)
                                {
                                    QString infoMessage("Nu s-au putut șterge toate fișierele selectate!");
                                    ui->labelInfo->setText(infoMessage);
                                }


                        }
                    else
                        {
                            QString infoMessage("Serverul nu a putut prelua comanda!");
                            ui->labelInfo->setText(infoMessage);
                        }

                }
        }

}


void CommandModeWindow::on_logoutButton_clicked()
{

    CloseDialog checkClose;
    checkClose.show();
    checkClose.exec();

    if (checkClose.getCloseResult())
        {
            ui->labelInfo->clear();
            if(this->isVisible())
                {
                    Communication commObj(serverDescriptor);

                    commObj.sendStringToDescriptor("logout");

                    string confirmation = commObj.recvStringFromDescriptor();


                    if(confirmation.substr(0,7) == "SUCCESS")
                        {
                            logout = true;
                            this->hide();
                            loginW->show();
                        }
                    else
                        {
                            QString infoMessage("A aparut o eroare! Încercați din nou!");
                            ui->labelInfo->setText(infoMessage);
                        }

                }
        }

}


void CommandModeWindow::on_quitButton_clicked()
{
    if(this->isVisible())
        this->close();
}


void CommandModeWindow::on_permButton_clicked()
{
    ui->labelInfo->clear();
    if(this->isVisible())
        {
            QList<QListWidgetItem*> selectedItems = ui->fileExplorer->selectedItems();

            if(selectedItems.size() == 0)
                {
                    QString infoMessage("Vă rugăm să selectați măcar un fișier pentru oferire permisiuni!");
                    ui->labelInfo->setText(infoMessage);
                }
            else
                {
                    Communication commObj(serverDescriptor);

                    nfd.clearString();
                    nfd.show();
                    nfd.exec();

                    QString userToAdd = nfd.getFileName();

                    if(userToAdd.length() == 0)
                        {
                            return;
                        }


                    commObj.sendStringToDescriptor("permissions");

                    string confirmation = commObj.recvStringFromDescriptor();

                    if(confirmation.substr(0,5) == "Ready")
                        {
                            commObj.sendStringToDescriptor(userToAdd.toStdString());
                            QString noFiles = QString::number(selectedItems.size());
                            commObj.sendStringToDescriptor(noFiles.toStdString());

                            bool error = false;
                            QString fileName;

                            foreach(QListWidgetItem * item, selectedItems)
                                {
                                    fileName = item->text();
                                    commObj.sendStringToDescriptor(fileName.toStdString());
                                    confirmation = commObj.recvStringFromDescriptor();
                                    if(confirmation.substr(0,5) != "Done!")
                                        {
                                            error = true;
                                        }

                                }
                            if(error)
                                {
                                    QString infoMessage("Nu s-au putut oferi permisiuni pentru toate fișierele selectate!");
                                    ui->labelInfo->setText(infoMessage);
                                }


                        }
                    else
                        {
                            QString infoMessage("Serverul nu a putut prelua comanda!");
                            ui->labelInfo->setText(infoMessage);
                        }

                }
        }

}
