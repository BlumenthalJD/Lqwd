#include "coreprocess.h"

CoreProcess::CoreProcess(QObject *parent) :
    QObject(parent)
{
    errM.consoleOut("Core Process Initiated");

    // Create the process, and setup error handling
    proc = new QProcess;
    connect(proc, SIGNAL(error(QProcess::ProcessError)), &errM, SLOT(handleProcessError(QProcess::ProcessError)));

    // Test to see what working dir to start off in
    if (OSINDEX != -1)
        (OSINDEX == 1) ? setWorkingDirectory("C:\\") :
                         setWorkingDirectory("/");
    else
        errM.catchError("Unknown OS", -1);
}

void CoreProcess::processCommand(QString command)
{
    QString result;
    QString translation = nexus.translateCommand(command);

    // Make sure translation is defined
    if( translation != REF_UNDEFINED )
    {
        switch(OSINDEX)
        {
        case -1:
            errM.catchError("Core Process - Unknown OS", -1);
            break;
        case 1:
            // Run command via windows interface
            result = winInterface(translation);
            break;
        case 2:
            // Run command via nix Interface
            result = nixInterface(translation);
            break;
        }
        emit processComplete(result);
    }
    else
    {
        errM.catchError("Core Process - Unknown primary command", 1);

        // This doesn't really need to be tested, but I do so just in case.
        if( command.split(' ').length() >= 1 )
            emit processComplete("Unknown Command [" + command.split(' ')[0] + "]");
        else
            emit processComplete("Unknown Command");
    }
}

QString CoreProcess::changeDirectory(QStringList command)
{
    // Handle moving up or down directories such as .. , and ../ as well as
    // ../home ../../etc

    QString response;

    switch(OSINDEX)
    {
        case 1:
        {
           //response = " WINDOWS DIR MOVE ";
           break;
        }
        case 2:
        {
            // See if new dir is ../ , if so generate the QDir path
            errM.catchError(" coreMod doCommand -> ONLY SUPPORTS ../ ATM , FIND WAY TO ../../../ n ");
            QString newDir = "";
            if( command[1] == "../" )
            {
                QDir curr = QDir::current();
                curr.cdUp();
                newDir = curr.path();
            }
            else
                newDir = command[1];

            // If the folder exists, move to it
            if( QDir(newDir).exists() )
            {
                setWorkingDirectory(newDir);
                return "Directory Changed, now in [" + newDir +"]";
            }
            else
                return ("[" + command[1] + "] is not a folder.");
           break;
        }
    }
    return "There was an error changing directories ";
}

void CoreProcess::setWorkingDirectory(QString dir)
{
    proc->setWorkingDirectory(dir);
    QDir::setCurrent(dir);
}

QString CoreProcess::nixInterface(QString command)
{
    QString response;
    QStringList cmdChunks = command.split(' ');

    /*
        Find a way to pull cd / chdir command from settings as to not
        have this bit be hard coded
    */
    if( cmdChunks[0] == "cd" && cmdChunks.length() > 1 )
    {
        response = changeDirectory(cmdChunks);
    }
    else
    {
        response = " LINUX COMMAND \n";
        proc->start(command);
        proc->waitForFinished(-1);
        QByteArray result = proc->readAllStandardOutput();
        QString res(result);
        response += res;
        proc->close();
    }

    errM.messageBox("ls 'ing a directory other than current is not working");
    qDebug() << command;

    return response;
}

QString CoreProcess::winInterface(QString)
{
    errM.catchError("Windows commands not yet programmed !", -1);
    return " WINDOWS COMMAND";
}

