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
    // Handles if html(1) or plain text(!1) output,
    outputType = false;

    QString result;
    QStringList translation = nexus.retrieveCommand(command);

    if( translation[0] != "?" )
    {
        if( translation[0] == "C" )
        {
            translation.removeAt(0);
            result = qtInterface(translation);
        }
        else if ( translation[0] == "T" )
        {
            translation.removeAt(0);
            switch(OSINDEX)
            {
            case -1:
                errM.catchError("Core Process - Unknown OS", -1);
                break;
            case 1:
                // Run command via windows interface
                //result = winInterface(translation[1]);
                break;
            case 2:
                // Run command via nix Interface
                result = nixInterface(translation);
                break;
            }
        }
        emit processComplete(result, outputType);
    }
    else
    {
        errM.catchError("Core Process - Unknown primary command", 1);

        // This doesn't really need to be tested, but I do so just in case.
        if( command.split(' ').length() >= 1 )
            emit processComplete("Unknown Command [" + command.split(' ')[0] + "]", outputType);
        else
            emit processComplete("Unknown Command", outputType);
    }
}

QString CoreProcess::qtInterface(QStringList cmdChunks)
{
    if( "list_directory" == cmdChunks[0] )
    {
        outputType = 1;
        unsigned i = 1;
        QString temp = "";
        QDir recoredDir(currDir);
        temp += "<font color=red>[Folders]</font><br><b>";

        // Get and display folders
        foreach(QString el, recoredDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs))
            ( i++ % 3 == 0 ) ? temp += (" " + el + "<br>") : temp += ( " " + el + "<font color=red>,</font>");\

        temp += "</b><br><font color=red>[Files]</font><br><i>";
        i = 1;

        // Get and display files
        foreach(QString el, recoredDir.entryList(QDir::NoDotAndDotDot | QDir::Files))
            ( i++ % 3 == 0 ) ? temp += (" " + el + "<br>") : temp += ( " " + el + "<font color=red>,</font>");\

        temp += "</i>";
        return temp;
    }
    else if( "change_directory" == cmdChunks[0] )
    {
        if(cmdChunks.length() <= 2)
        {
            setWorkingDirectory(QDir::homePath());
            return "";
        }
        else
            return changeDirectory(cmdChunks);
    }
    else if( "remove_document" == cmdChunks[0] )
    {

    }
    else if( "clear_console" == cmdChunks[0] )
    {

    }
    else
    {
        return "Unhandled command scope.";
    }


    qDebug() << "QT INTERFACE" << cmdChunks;
    return "QTINTERFACE";
}

QString CoreProcess::nixInterface(QStringList cmdChunks)
{
    QString response, command;
    for(int i = 0; i < cmdChunks.length(); i++)
    {
        command += cmdChunks[i];
        if( i != cmdChunks.length()-1 )
            command += " ";
    }

    response = " LINUX COMMAND \n";
    proc->start(command);
    proc->waitForFinished(-1);
    QByteArray result = proc->readAllStandardOutput();
    QString res(result);
    response += res;
    proc->close();


    return response;
}

QString CoreProcess::winInterface(QStringList)
{
    errM.catchError("Windows commands not yet programmed !", -1);
    return " WINDOWS COMMAND";
}


void CoreProcess::setWorkingDirectory(QString dir)
{
    proc->setWorkingDirectory(dir);
    QDir::setCurrent(dir);
}

QString CoreProcess::changeDirectory(QStringList command)
{
    command.removeAt(0);
    // Handle moving up or down directories such as .. , and ../ as well as
    // ../home ../../etc

    QString response;

    /*
        // See if the command is a simple ..
        if ( cmdChunks[2].length() == 2 && cmdChunks[2][cmdChunks[2].length()-1] == '.')
        {
            qDebug() << " Move one up";
        }

        for( int i = 0; i < cmdChunks[2].length(); i++ )
        {
            if( cmdChunks[2][i] == '.' && i == 0)
            {

            }
        }


*/

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


    return "There was an error changing directories ";
}
