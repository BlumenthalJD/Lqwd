#include "engine.h"
#include <QDebug>
#include <QDir>

Engine::Engine(QObject *parent) :
    QObject(parent)
{
    if( OSINDEX == -1 )
        errM.catchError("Unknown OS", -1);
    else
        loadCommandMap();



    errM.catchError(" TO DO : \n Program : Engine::doCommand()  , Console arrow / history functionality\n\n Also : Check out command ls -la - Something is wrong");

}

void Engine::loadCommandMap()
{
    QFile inputFile(":/config/command_map.lqwd");
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
void Engine::insertIntoMap(QString cmd)
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
    cmdData.insertCommand(exportCommands, exportArguments);
}

/*
    Slot that recieves the command from the user
*/
void Engine::processCommand(QString cmd)
{

    // Translate the filtered input to the mapped command and args
   QString command = cmdData.retrieveCommand(commandFilter(cmd));

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
QStringList Engine::commandFilter(QString input)
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
QString Engine::doCommand(QString cmd)
{
    QProcess run;

    if( OSINDEX == 1 )
    {
        // Special remove case
    }
    else
    {

    }
    return (" Translated to : " + cmd);
}
