#include "corenexus.h"

CoreNexus::CoreNexus()
{
    loadCommandMap();
    ignoreExplicitlyUndefinedArguments = true;
}

CoreNexus::~CoreNexus(){}

void CoreNexus::loadCommandMap()
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

void CoreNexus::insertIntoMap(QString cmd)
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
    commandMap.append(_cmap(exportCommands, exportArguments));
}

QStringList CoreNexus::filterCommand(QString input)
{
    QStringList cmd;
    QString buffer = "";
    bool ignore = false, spaces = false;

    foreach(QString ch, input)
    {
        if( ch == " " && !ignore)
        {
            cmd.append(buffer);
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
            cmd.append(buffer);
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
            cmd.append(buffer);
            buffer = "";
        }
    }

    // If there is still something in the buffer, and its not a single command
    if( buffer != "" && input.split(" ").length() > 1)
    {
        cmd.append(buffer);

    }

    // if it is a simple command, and nothing trailing
    if(!spaces)
        cmd.append(input);

    // Just in case any slip by
    cmd.removeAll("");
    return cmd;
}

QString CoreNexus::translateCommand(QString input)
{
    errM.consoleOut(" CoreNexus Filtering");
    /*
        Filter the input to seperate out command and arguments
    */
    QStringList cmd = filterCommand(input);


    errM.consoleOut(" CoreNexus Translating");
    /*
            Perform actual translation
    */
    for( int i = 0; i < commandMap.count(); i++)
    {
        // Find and translate the command they want to run
       if(commandMap[i].commandList[0] == cmd[0])
       {
            // temp qslist that will hold translations
            QStringList temp;
            temp.append(commandMap[i].commandList[OSINDEX]);

            // List of translated cmd indexes
            QList<int> added;

            // Go through remaining arguments in ret
            for(int j = 1; j < cmd.count(); j++)
            {
                if( commandMap[i].argumentMap.count() > 0 )
                {
                    // Compair against the first entry of every arg map for that command
                    for(int k = 0; k < commandMap[i].argumentMap.count(); k++)
                    {
                        // If an argument for the command has a translation for the current os
                        // add it, if that translation is explicitly undefined, ignore it.
                        if( commandMap[i].argumentMap[k][0] == cmd[j] )
                        {
                            // If the current index has not been translated
                            if( !added.contains(j))
                            {
                                added.append(j);
                                // If argument is explicitly undefined
                                if( commandMap[i].argumentMap[k][OSINDEX] == REF_UNDEFINED )
                                {
                                    // If we aren't ignoring the command, add it
                                    if( !ignoreExplicitlyUndefinedArguments )
                                    {
                                        temp.append(cmd[j]);
                                    }
                                }
                                else
                                {
                                    // Argument not explictly undefined, add translation
                                    temp.append(commandMap[i].argumentMap[k][OSINDEX]);
                                }
                            }
                        }
                        else
                        {
                            if( !added.contains(j))
                            {
                                // Add arg as-is, not defined
                                temp.append(cmd[j]);
                                added.append(j);
                            }
                        }
                    }
                }
                // There are no arguments given for the command, so pass it along
                else
                {
                    if( !added.contains(j))
                    {
                        // Add arg as-is, not defined
                        temp.append(cmd[j]);
                        added.append(j);
                    }
                }
            }

            // All arguments have been added, so make the temp list into a string to execute
            QString translation = "";
            for(int jb = 0; jb < temp.count(); jb++)
            {
                translation += temp[jb];
                if(jb != temp.count()-1)
                    translation += " ";
            }
            // Return the translated command
            errM.consoleOut(" CoreNexus Translated " + input + " to " + translation);
            return translation;
       }
    }
    // Command not found
    return REF_UNDEFINED;

}
