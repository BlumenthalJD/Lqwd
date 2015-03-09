#ifndef ERRORMACHINE_H
#define ERRORMACHINE_H

#include <QObject>
#include "defines.h"

class ErrorMachine : public QObject
{
    Q_OBJECT
public:
    explicit ErrorMachine(QObject *parent = 0);

signals:

public slots:

    void catchError(QString, int arg=0);

};

#endif // ERRORMACHINE_H
