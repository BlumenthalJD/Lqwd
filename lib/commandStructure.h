#ifndef COMMANDSTRUCTURE_H
#define COMMANDSTRUCTURE_H

#include <QList>
#include <QDebug>
#include <QString>
#include <QStringList>
#include "defines.h"

// I made this into a structure so
// specific command settings could
// easily be added later
struct cmap
{
    // 0 = Command, 1 = Windows Command, 2 = Nix Command
    QStringList commandList;
    QList<QStringList> argumentMap;

    cmap(QStringList cmds, QStringList args)
    {
        // Add the commands to the command list
        commandList = cmds;
        // Add each argument and their corresponding os argument to argmap
        foreach(QString a, args)
        {
            argumentMap.append(a.split(DELIMITER_BLOCK));
        }
    }
};
typedef struct cmap _cmap;

/*
    In the future if this becomes slow due to a large command map,
    it may be benificial to implement a transpose-search list.
*/

struct commandStruct
{
    // The command map
    QList<_cmap> commandMap;

    // If argument that is mapped for an os, but explicitly the current one
    // ( indicated by ! in command_map.lqwd ) it should be ignored when a user puts it in,
    // unless they switch this to false
    bool ignoreExplicitlyUndefinedArguments;

    commandStruct()
    {
        ignoreExplicitlyUndefinedArguments = true;
    }

    // Add command to the map
    void insertCommand(QStringList cmd, QStringList args)
    {
        // Create a _cmap and sent to command map list
        commandMap.append(_cmap(cmd, args));
    }

    /*
        QString translateCommand(QStringList ret)

        Takes in the user's input and translates it to the specific os
        parameters in command_map.lqwd. All commands must be defined in
        the command_map.lqwd config file, or they will be undefined.
        If not mapped, arguments will be passed as-is assuming the user
        knows what they are doing
    */
    QString translateCommand(QStringList ret)
    {
        for( int i = 0; i < commandMap.count(); i++)
        {
            // Find and translate the command they want to run
           if(commandMap[i].commandList[0] == ret[0])
           {
                // temp qslist that will hold translations
                QStringList temp;
                temp.append(commandMap[i].commandList[OSINDEX]);

                // List of translated ret indexes
                QList<int> added;

                // Go through remaining arguments in ret
                for(int j = 1; j < ret.count(); j++)
                {
                    // Compair against the first entry of every arg map for that command
                    for(int k = 0; k < commandMap[i].argumentMap.count(); k++)
                    {
                        // If an argument for the command has a translation for the current os
                        // add it, if that translation is explicitly undefined, ignore it.
                        if( commandMap[i].argumentMap[k][0] == ret[j] )
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
                                        temp.append(ret[j]);
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
                                temp.append(ret[j]);
                                added.append(j);
                            }
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
};
typedef commandStruct _cmdStruct;

#endif // COMMANDSTRUCTURE_H
