/****************************************************************************
**
**      Author : Josh Bosley
**
****************************************************************************/

#include"lib/engine.h"
#include "widgets/console/bkons.h"

#include <QDebug>
#include <QScrollBar>


Bkons::Bkons(QWidget *parent)
    : QPlainTextEdit(parent)
{
    connect(this, SIGNAL(dataReady(QString)), &engine, SLOT(processCommand(QString)));
    connect(&engine, SIGNAL(responseReady(QString)), this, SLOT(responseOut(QString)));

    buffer = "";
    hIndex = 0;
    maxHistory = 1000;
    document()->setMaximumBlockCount(1000);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);
}

void Bkons::beginSession()
{
    active = true;
    lqwdText = "@lqwd>  ";
    currentActivity = "core";
    appendPlainText(currentActivity + lqwdText);
    setEnabled(true);
}

void Bkons::puts(QString data)
{
    // Insert text, and scroll screen to bottom
    insertPlainText("\n" + data);
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
    // Add the activity id
    appendPlainText(currentActivity + lqwdText);
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
            insertPlainText( "\n" + currentActivity + lqwdText + buffer);
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

void Bkons::addKeyToBuffer(QString key)
{
    buffer += key;
    insertPlainText(key);
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

