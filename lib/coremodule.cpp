#include "coremodule.h"
#include <QDebug>
#include <QDir>

CoreEngine::CoreEngine(QObject *parent) :
    QObject(parent)
{
    // Load the config file to command nexus ( move this to nexus )
    loadCommandMap();

    // Load the UI settings
    settings.loadFile(":/config/coreModule/core_config.lqwd");

    // Create the process, and setup error handling
    proc = new QProcess;
    connect(proc, SIGNAL(error(QProcess::ProcessError)), &errM, SLOT(handleProcessError(QProcess::ProcessError)));

    // Setup os specifics
    switch(OSINDEX)
    {
        case -1:
            errM.catchError("Unknown OS", -1);
        break;
        case 1:
        {
            proc->setWorkingDirectory("C:\\");
            QDir::setCurrent("C:\\");
            break;
        }
        case 2:
        {
            proc->setWorkingDirectory("/");
            QDir::setCurrent("/");
            break;
        }
    }
}

void CoreEngine::loadCommandMap()
{
    QFile inputFile(":/config/coreModule/command_map.lqwd");
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
           QString ln = in.readLine();
           if( ln != "" )
               if( ln != QString(REF_COMMENT))
                   if(ln.at(0) != REF_COMMENT)
                   {
                       insertIntoMap(ln);
                   }
       }
       inputFile.close();
    }
    else
    {
        errM.catchError(" Could not open resource file : command_map.lqwd ", -1);
    }
}

/*
    Chop up input fromt he config file, and create the command maps
*/
void CoreEngine::insertIntoMap(QString cmd)
{
    QStringList exportCommands, exportArguments, tempArguments;

    // 0 = Commands, >0 = Arguments
    QStringList cmdBlocks = cmd.split(DELIMITER_LIST);

    for(int h = 0; h < cmdBlocks.count(); h++)
    {
        QStringList commands = cmdBlocks[h].split(DELIMITER_BLOCK);

        (h == 0) ? exportCommands.append(commands[0]) : tempArguments.append(commands[0]);

        for( int i = 1; i < commands.length(); i++ )
        {
           if( commands[i] == REF_PRIOR )
           {
               if( commands[i-1] == REF_PRIOR)
               {
                   (h == 0) ? exportCommands.append(commands[i-2]) : tempArguments.append(commands[i-2]);
               }
               else if (commands[i-1] == REF_UNDEFINED)
               {
                   (h == 0) ? exportCommands.append(commands[i-2]) : tempArguments.append(commands[i-2]);
               }
               else
               {
                   (h == 0) ? exportCommands.append(commands[i-1]) : tempArguments.append(commands[i-1]);
               }
           }
           else if( commands[i] == REF_UNDEFINED)
           {
               (h == 0) ? exportCommands.append(REF_UNDEFINED) : tempArguments.append(REF_UNDEFINED);
           }
           else
           {
               (h == 0) ? exportCommands.append(commands[i]) : tempArguments.append(commands[i]);
           }
        }
    }

    // Once all arguments and commands are stored, seperate indidvidual arguments, and their maps out
    // so they can be stored for reference
    QString temp = "";
    for( int i = 0, c = 0; i < tempArguments.count(); i++ )
    {
        // There will only be commands for the n of differ OS specified.
        if( c < exportCommands.count())
        {
            temp += tempArguments[i];
            c++;
            if( c < exportCommands.count())
                temp += DELIMITER_BLOCK;
        }
        // Once the n is reached, the next n items will a different argument type.
        if ( c >= exportCommands.count())
        {
            exportArguments.append(temp);
            temp = "";
            c = 0;
        }
    }
    // Send the commands and arguments to be saved
    cmdNexus.addCommandToMap(exportCommands, exportArguments);
}

/*
    Slot that recieves the command from the user
*/
void CoreEngine::processCommand(QString cmd)
{

    // Translate the filtered input to the mapped command and args
   QString command = cmdNexus.translateCommand(commandFilter(cmd));

    if( command == REF_UNDEFINED )
    {
        /*                             TO DO > HERE
         *
         *  Set the activity to cmdResponse, ask if they want to map. Handle response
         *
         */
        emit responseReady("[ " + cmd.split(" ")[0] + " ] Is not mapped. - New map setup not created yet");
    }
    else
    {
        // Do the command they asked for. Do command will return the output to the emitter
        emit responseReady(doCommand(command));
    }
}

/*
    Filter out the arguments and the variables from input and return info
*/
QStringList CoreEngine::commandFilter(QString input)
{
    QString buffer = "";
    QStringList pieces;
    bool ignore = false, spaces = false;

    foreach(QString ch, input)
    {
        if( ch == " " && !ignore)
        {
            pieces.append(buffer);
            buffer = "";

            if(!spaces)
                spaces = true;
        }
        else if ( ch == QString('"') && !ignore )
        {
            // Find ' " ' encased strings ex:  "Monty"
            buffer += ch;
            ignore = true;

        }
        else if ( ch == QString('"') && ignore)
        {
            // Get end of encased strings ex:  "Monty"
            buffer += ch;
            ignore = false;
            pieces.append(buffer);
            buffer = "";
        }
        else if(ignore || ch != " ")
        {
            // If its a blank space and its in an encased string, add it
            // if its a blank space and not encased, ignot it
            buffer += ch;
        }

        // Attach last char
        if(input.indexOf(ch) == input.length())
        {
            pieces.append(buffer);
            buffer = "";
        }
    }

    // If there is still something in the buffer, and its not a single command
    if( buffer != "" && input.split(" ").length() > 1)
    {
        pieces.append(buffer);

    }

    // if it is a simple command, and nothing trailing
    if(!spaces)
        pieces.append(input);

    // Just in case any slip by
    pieces.removeAll("");
    return pieces;
}


/*                             TO DO > HERE
 *
 *  Handle the execution of the command, and get the output from the system for response.
 *
 */


/*
    Once the command is processed, and if mapped will be sent to be executed.
*/
QString CoreEngine::doCommand(QString cmd)
{
    QString response = "";
    QStringList cmdChunks = cmd.split(" ");

    errM.catchError(" TODO : WORK ON DOCOMMAND ");

    switch(OSINDEX)
    {
        case 1:
        {
            response = " WINDOWS COMMAND ";
            break;
        }
        case 2:
        {
        // Figure out a better way to test for things like cd
            if( cmdChunks[0] == "cd" && cmdChunks.length() > 1 )
            {
                // See if new dir is ../ , if so generate the QDir path
                errM.catchError(" coreMod doCommand -> ONLY SUPPORTS ../ ATM , FIND WAY TO ../../../ n ");
                QString newDir = "";
                if( cmdChunks[1] == "../" )
                {
                    QDir curr = QDir::current();
                    curr.cdUp();
                    newDir = curr.path();
                }
                else
                    newDir = cmdChunks[1];

                // If the folder exists, move to it
                if( QDir(newDir).exists() )
                {
                    proc->setWorkingDirectory(newDir);
                    QDir::setCurrent(newDir);
                }
                else
                    response += (cmdChunks[1] + " is not a folder.");
            }
            else
            {
                response =+ " LINUX COMMAND \n";
                proc->start(cmd);
                proc->waitForFinished(-1);
                QByteArray result = proc->readAllStandardOutput();
                QString res(result);
                response += res;
                proc->close();
            }
            break;
        }
    }

    return ("\t\t\t Response From : " + cmd + "\n" + response);
}
