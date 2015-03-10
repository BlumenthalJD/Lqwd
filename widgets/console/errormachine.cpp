#include<QDebug>
#include "errormachine.h"

ErrorMachine::ErrorMachine(QObject *parent) :
    QObject(parent)
{
    catchError("ErrorMachine Started");
}

void ErrorMachine::catchError(QString err, int arg)
{
    qDebug() << "CAUGHT: " << err ;
    if( arg != 0 )
        qDebug() << "ARG : " << arg;
}

void ErrorMachine::handleProcessError(QProcess::ProcessError e)
{
    qDebug() << e;
}
