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

    // Add command to the map
    void insertCommand(QStringList cmd, QStringList args)
    {
        // Create a _cmap and sent to command map list
        commandMap.append(_cmap(cmd, args));
    }

    // Retrieve command from map and iterate through map
    QString retrieveCommand(QStringList ret)
    {
        for( int i = 0; i < commandMap.count(); i++)
        {
            // Find the command they want to run
            // Once the command is found, translate it's arguments
           if(commandMap[i].commandList[0] == ret[0])
           {
                QStringList temp;
                // Add translated command to the temp list.
                temp.append(commandMap[i].commandList[OSINDEX]);
                QList<int> added;
                // Go through remaining arguments in ret
                for(int j = 1; j < ret.count(); j++)
                {
                    // Compair against the first entry of every arg map for that command
                    for(int k = 0; k < commandMap[i].argumentMap.count(); k++)
                    {
                        // If an argument for the command has a translation add
                        // the translation to the temp list, if not add it to temp
                        if( commandMap[i].argumentMap[k][0] == ret[j] )
                        {
                            if( !added.contains(j))
                            {
                                temp.append(commandMap[i].argumentMap[k][OSINDEX]);
                                added.append(j);
                            }
                        }
                        else
                        {
                            if( !added.contains(j))
                            {
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
