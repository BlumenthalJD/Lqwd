/****************************************************************************
**
**      Author : Josh Bosley
**
****************************************************************************/

#include "widgets/console/bkons.h"

#include <QScrollBar>

#include <QtCore/QDebug>

Bkons::Bkons(QWidget *parent)
    : QPlainTextEdit(parent)
{
    buffer = "";
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
    lqwdText = "@lqwd> :";
    currentActivity = "core";
    currentLine = 0;
    appendPlainText(currentActivity + "@lqwd> :");
    setEnabled(true);

}

void Bkons::handleUserInput(QString in)
{
    puts(in);
}

void Bkons::puts(QString data)
{
    insertPlainText(data);
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Bkons::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Backspace:
    {
        if(e->text().length() >= (currentActivity.length() + lqwdText.length()))
        {
            // Delete the last char from buffer, and redisplay // textCursor().deletePreviousChar();
        }
        break;
    }
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if( buffer != "")
            initiateCommand();
        break;

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

void Bkons::initiateCommand()
{
    QString cmd = buffer;
    buffer = "";



    appendPlainText("RESPNSE TO : " + cmd);

    appendPlainText(currentActivity + lqwdText);


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

