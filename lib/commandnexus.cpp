#include "commandnexus.h"

CommandNexus::CommandNexus()
{
    ignoreExplicitlyUndefinedArguments = true;
}

CommandNexus::~CommandNexus(){}

void CommandNexus::addCommandToMap(QStringList cmd, QStringList args)
{
    // Create a _cmap and sent to command map list
    commandMap.append(_cmap(cmd, args));
}

QString CommandNexus::translateCommand(QStringList cmd)
{
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

            return translation;
       }
    }
    // Command not found
    return REF_UNDEFINED;

}
