#ifndef ENGINE_H
#define ENGINE_H

#include <QHash>
#include <QObject>
#include <QProcess>
#include "defines.h"
#include "errormachine.h"
#include "commandStructure.h"

class Engine : public QObject
{
    Q_OBJECT
public:
    explicit Engine(QObject *parent = 0);

signals:
    void responseReady(QString);

public slots:
    void processCommand(QString);

private:
    QHash<QString, int> coreCmds;

    unsigned osIndex;
    ErrorMachine errM;
    _cmdStruct cmdData;

    void loadCommandMap();
    void insertIntoMap(QString);
    QString doCommand(QString);

    // Takes in users command and seperates out arguments
    QStringList commandFilter(QString);
};

#endif // ENGINE_H
