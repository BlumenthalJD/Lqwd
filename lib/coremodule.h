#ifndef LQWD_CORE_MODULE
#define LQWD_CORE_MODULE

#include <QHash>
#include <QObject>
#include <QProcess>

// Retrieves and translates commands
#include "commandnexus.h"

// For use of console defines, and error handling
#include "../widgets/console/defines.h"
#include "../widgets/console/errormachine.h"
#include "../widgets/console/consolesettings.h"

class CoreEngine : public QObject
{
    Q_OBJECT
public:
    explicit CoreEngine(QObject *parent = 0);
    int moduleId;
    // Settings
    ConsoleSettings settings;

signals:
    void responseReady(QString);

public slots:
    void processCommand(QString);

private:
    QHash<QString, int> coreCmds;

    // Error handler
    ErrorMachine errM;

    // Command Translator
    CommandNexus cmdNexus;

    // Takes in users command and seperates out arguments
    QStringList commandFilter(QString);

    // Read in the command file, and insert into command map
    void loadCommandMap();
    void insertIntoMap(QString);

    // Inititate the command, and display output
    QString doCommand(QString);

    QProcess * proc;
};

#endif // LQWD_CORE_MODULE
