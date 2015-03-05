#ifndef ENGINE_H
#define ENGINE_H

#include <QHash>
#include <QObject>
#include "errormachine.h"

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

    ErrorMachine errM;
};

#endif // ENGINE_H
