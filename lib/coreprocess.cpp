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

QString CoreProcess::getCWD()
{
    return currDir;
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
            result = systemInterface(translation);
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
        QStringList directoryList = recoredDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs);
        QStringList documentList = recoredDir.entryList(QDir::NoDotAndDotDot | QDir::Files);

        // Get and display directories
        temp += "<font color=red>[Directories]</font><br><b>";

        foreach(QString el, directoryList)
            if( i++ % 3 == 0 )
                temp += (" " + el + "<br>");
            else
            {
                temp += ( " " + el);

                // If its not the last entry, add a red comma
                if( !((directoryList.indexOf(el)) == (directoryList.length()-1)))
                    temp += ("<font color=red>,</font>");
            }

        // Get and display documents
        temp += "</b><br><br><font color=red>[Documents]</font><br>";
        i = 1;

        // Get and display documents
        foreach(QString el, documentList)
            if ( i++ % 3 == 0 )
                temp += (" " + el + "<br>");
            else
            {
                temp += ( " " + el);

                // If its not the last entry, add a red comma
                if( !((documentList.indexOf(el)) == (documentList.length()-1)))
                    temp += ("<font color=red>,</font>");
            }

        return temp;
    }
    else if( "change_directory" == cmdChunks[0] )
    {

        qDebug() << cmdChunks;
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
        return "[coreProcess] Directory removal not programmed";
    }
    else if( "clear_console" == cmdChunks[0] )
    {
        return cmdChunks[0];
    }
    else if("create_directory" == cmdChunks[0] )
    {
        return "[coreProcess] Directory creation not programmed";
    }
    else if("load_module" == cmdChunks[0] )
    {
        return "[coreProcess] Module loading not programmed";
    }
    else
    {
        return "Unhandled command scope.";
    }
}

QString CoreProcess::systemInterface(QStringList cmdChunks)
{
    QString response, command;
    for(int i = 0; i < cmdChunks.length(); i++)
    {
        command += cmdChunks[i];
        if( i != cmdChunks.length()-1 )
            command += " ";
    }

    proc->start(command);
    proc->waitForFinished(-1);
    QByteArray result = proc->readAllStandardOutput();
    QString res(result);
    response += res;
    proc->close();

    return response;
}

void CoreProcess::setWorkingDirectory(QString dir)
{
    proc->setWorkingDirectory(dir);
    QDir::setCurrent(dir);
}

QString CoreProcess::changeDirectory(QStringList command)
{
    command.removeAt(0);
    QString newDir = "";
    if( command[1] == ".." )
    {
        QDir curr = QDir::current();
        curr.cdUp();
        newDir = curr.path();
    }
    else
        newDir = command[1];

    // Remove " in "path name"
    QString temp = "";
    foreach(QString ch, newDir)
    {
        if(ch != QString('"'))
            temp += ch;
    }
    newDir = temp;

    // If the folder exists, move to it
    if( QDir(newDir).exists() )
    {
        setWorkingDirectory(newDir);
        return "change_directory";
    }
    else
        return ("[" + command[1] + "] is not a folder.");

    return "There was an error changing directories ";
}
