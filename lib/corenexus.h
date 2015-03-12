#ifndef COMMANDNEXUS_H
#define COMMANDNEXUS_H

#include <QFile>
#include <QList>
#include <QDebug>
#include <QString>
#include <QStringList>

#include "../widgets/console/defines.h"
#include "../widgets/console/errormachine.h"

// The map structure for commands and arguments
struct cmap
{
    // 0 = Command, 1 = Windows Command, 2 = Nix Command (defines.h -> IOSINDEX)
    QStringList commandList;
    QList<QStringList> argumentMap;
    cmap(QStringList cmds, QStringList args)
    {
        commandList = cmds;
        foreach(QString a, args)
            argumentMap.append(a.split(DELIMITER_BLOCK));
    }
};
typedef struct cmap _cmap;

class CoreNexus
{
public:
    CoreNexus();
    ~CoreNexus();

    QString translateCommand(QString cmd);

private :
    ErrorMachine errM;
    QList<_cmap> commandMap;

    // Explicitly undefined os arguments (indicated by ! in command_map.lqwd)
    // will be ignored, flip bool to true to allow
    bool ignoreExplicitlyUndefinedArguments;

    void loadCommandMap();
    void insertIntoMap(QString);
    void addCommandToMap(QStringList cmd, QStringList args);
    QStringList filterCommand(QString);
};

#endif // COMMANDNEXUS_H
