/****************************************************************************
**
**      Author : Josh Bosley
**
****************************************************************************/

#ifndef CONSOLE_BKONS
#define CONSOLE_BKONS

#include <QPlainTextEdit>


class Bkons : public QPlainTextEdit
{
    Q_OBJECT

signals:
    void retData(const QByteArray &data);

public:
    explicit Bkons(QWidget *parent = 0);

    void beginSession();
    void handleUserInput(QString);
    void puts(QString);

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);

private:
    int currentLine;
    bool active;
    QString lqwdText,currentActivity;

    // Input Controls
    QString buffer;
    QStringList history;
    int maxHistory;

    void addKeyToBuffer(QString key);
    void initiateCommand();




};

#endif // CONSOLE_H
