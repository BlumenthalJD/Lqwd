#include <QDebug>
#include <QScrollBar>
#include"lib/engine.h"
#include "widgets/console/bkons.h"

Bkons::Bkons(QWidget *parent)
    : QPlainTextEdit(parent)
{
    // Connect the console to the engine for communication.
    connect(this, SIGNAL(dataReady(QString)), &engine, SLOT(processCommand(QString)));
    connect(&engine, SIGNAL(responseReady(QString)), this, SLOT(responseOut(QString)));

    // Initialize input buffer, history index, and history size limit
    buffer = "";
    hIndex = 0;
    maxHistory = 1000;
    document()->setMaximumBlockCount(1000);

    // Color the screen, set the font
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setFont (QFont ("Courier", 13));
    setPalette(p);
}

void Bkons::beginSession()
{
    backSpaceNline = false;
    lqwdText = "@lqwd>  ";
    currentActivity = "core";
    getUserPrompt();
    setEnabled(true);
}

void Bkons::puts(QString data)
{
    // Insert nl + data, then scroll screen to bottom
    insertPlainText( "\n" + data);
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());

    getUserPrompt();
}

void Bkons::responseOut(QString response)
{
    puts(response);
}

void Bkons::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Backspace:
    {
        if(buffer.length() > 0)
        {
            setFocus();
            buffer.remove(buffer.length()-1,1);
            QTextCursor storeCursorPos = textCursor();
            moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
            textCursor().removeSelectedText();
            textCursor().deletePreviousChar();
            setTextCursor(storeCursorPos);
            getUserPrompt();
            insertPlainText(buffer);
        }
        break;
    }
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:

        /*

                NEEED TO FINDA  BETTER WAY TO SHOW THE HISTORY ON THE UP ARROW. USE HINDEX TO
                REMEMBER THE SPOT OF LAST VIEWED. RESET IT WHEN USER PRESSES ENTER

          */
        qDebug() << " NEED TO PROGRAM ARROW FEATURES ";
        /*
        setFocus();
        for(int i = 0; i < buffer.length(); i++)
        {
            QTextCursor storeCursorPos = textCursor();
            moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
            textCursor().removeSelectedText();
            textCursor().deletePreviousChar();
            setTextCursor(storeCursorPos);
        }
*/

        buffer = "";
        break;
    case Qt::Key_Down:
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    {
        if( !buffer.trimmed().isEmpty())
        {
            history.append(buffer);
            emit dataReady(buffer);
            buffer = "";
        }
        break;
    }

    default:
        addKeyToBuffer(e->text());
        break;
    }
}

void Bkons::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void Bkons::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void Bkons::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}

void Bkons::addKeyToBuffer(QString key)
{
    buffer += key;
    insertPlainText(key);
}

void Bkons::getUserPrompt()
{
    appendHtml("<font color=\"White\">" +
               currentActivity + lqwdText +
               "</font>");
}
