#ifndef CONSOLE_BKONS
#define CONSOLE_BKONS

#include "lib/engine.h"
#include <QPlainTextEdit>

class Bkons : public QPlainTextEdit
{
    Q_OBJECT

signals:
    void dataReady(QString data);

public:
    explicit Bkons(QWidget *parent = 0);

    void beginSession();
    void puts(QString);

public slots:

    void responseOut(QString);

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);

private:

    // For handling commands
    Engine engine;

    // State, and display
    int hIndex;
    QString lqwdText,
    currentActivity;

    // Input Controls
    QString buffer;
    int maxHistory;
    bool backSpaceNline;
    QStringList history;

    void addKeyToBuffer(QString key);

    void getUserPrompt();
};

#endif // CONSOLE_H
