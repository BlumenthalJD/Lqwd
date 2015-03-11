#ifndef COREPROCESS_H
#define COREPROCESS_H


#include <QDir>
#include <QObject>
#include <QProcess>

#include "corenexus.h"
#include "../widgets/console/defines.h"
#include "../widgets/console/errormachine.h"

class CoreProcess : public QObject
{
    Q_OBJECT
public:
    explicit CoreProcess(QObject *parent = 0);

signals:
    void processComplete(QString);

public slots:
    void processCommand(QString);

private:
    QProcess * proc;
    CoreNexus nexus;
    ErrorMachine errM;
    QString currDir;

    QString changeDirectory(QStringList);
    void setWorkingDirectory(QString);

    QString nixInterface(QString);
    QString winInterface(QString);
};

#endif // COREPROCESS_H
