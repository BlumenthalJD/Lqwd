#include "coremodule.h"

CoreModule::CoreModule(QObject *parent) :
    QObject(parent)
{
    errM.consoleOut("Core Module Inititated");

    // Load the UI settings
    settings.loadFile(":/config/coreModule/core_config.lqwd");

    // Connect to core process
    connect(this, SIGNAL(processCommand(QString)), &cProc, SLOT(processCommand(QString)));
    connect(&cProc, SIGNAL(processComplete(QString)), this, SLOT(processReturn(QString)));
}

/*
    Slot that recieves the command from the user
*/
void CoreModule::humanInput(QString cmd)
{
    emit processCommand(cmd);
}

/*
    Slot that recieves the response from the process
*/
void CoreModule::processReturn(QString res)
{
    emit responseReady(res);
}



