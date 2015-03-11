#include <QDebug>
#include <QScrollBar>
#include "widgets/console/bkons.h"

/*
    Module that interacts with the user
*/
#include"lib/coremodule.h"

Bkons::Bkons(QWidget *parent)
    : QPlainTextEdit(parent)
{
    errM.showMessage("Bkons Inititated");

    lqwdText = "@lqwd>  ";
    arrowIndex = 0;


    // In case module doesn't provide a setting - moduleId must be given.
    defaultSettings.loadFile(":/config/bkons/bkons_config.lqwd");

    // Initialize input buffer, history index, and history size limit
    buffer = "";

    /*
            Load the module
    */
    loadModule(&coreModule.settings);
}

void Bkons::loadModule(ConsoleSettings * settings)
{
    (settings->getValue("moduleId") == "default")
            ? currentModule = defaultSettings.getValue("moduleId").toInt()
            : currentModule = settings->getValue("moduleId").toInt();

    (settings->getValue("maxHistory") == "default")
            ? maxHistory = defaultSettings.getValue("maxHistory").toInt()
            : maxHistory = settings->getValue("maxHistory").toInt();

    (settings->getValue("maxBlockCount") == "default")
            ? maxBlockCount  = defaultSettings.getValue("maxBlockCount").toInt()
            : maxBlockCount = settings->getValue("maxBlockCount").toInt();

    (settings->getValue("fontSize") == "default")
            ? fontSize = defaultSettings.getValue("fontSize").toInt()
            : fontSize = settings->getValue("fontSize").toInt();

    (settings->getValue("font") == "default")
            ? font = defaultSettings.getValue("font")
            : font = settings->getValue("font");

    (settings->getValue("fontColor") == "default")
            ? fontColor = defaultSettings.getValue("fontColor")
            : fontColor = settings->getValue("fontColor");

    (settings->getValue("backgroundColor") == "default")
            ? backgroundColor  = defaultSettings.getValue("backgroundColor")
            : backgroundColor = settings->getValue("backgroundColor");

    (settings->getValue("promptColor") == "default")
            ? promptColor  = defaultSettings.getValue("promptColor")
            : promptColor = settings->getValue("promptColor");

    (settings->getValue("activityName") == "default")
            ? activityName  = defaultSettings.getValue("activityName")
            : activityName = settings->getValue("activityName");

    document()->setMaximumBlockCount(maxBlockCount);

    currentActivity = activityName;

    // Split the rgb values from settings, and load to palette
    QPalette p = palette();
    int r = backgroundColor.split(",")[0].toInt(),
        g = backgroundColor.split(",")[1].toInt(),
        b = backgroundColor.split(",")[2].toInt();
    p.setColor(QPalette::Base, QColor::fromRgb(r,g,b));

    r = fontColor.split(",")[0].toInt(),
    g = fontColor.split(",")[1].toInt(),
    b = fontColor.split(",")[2].toInt();
    p.setColor(QPalette::Text, QColor::fromRgb(r,g,b));

    setFont (QFont (font,fontSize));
    setPalette(p);

    // Using module ids for potential future expansions
    if( 0 == currentModule )
    {
        connect(this, SIGNAL(dataReady(QString)), &coreModule, SLOT(humanInput(QString)));
        connect(&coreModule, SIGNAL(responseReady(QString)), this, SLOT(responseOut(QString)));
    }
    else
        errM.catchError("Module could not be loaded", -1);
}

void Bkons::beginSession()
{
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
            QTextCursor storeCursorPos = textCursor();

            if( 0 == arrowIndex )
            {
                buffer.remove(buffer.length()-1,1);
                moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
                moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
                moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
                textCursor().removeSelectedText();
                textCursor().deletePreviousChar();
                getUserPrompt();
                insertPlainText(buffer);
            }
            else if ( 0 < arrowIndex )
            {
                if( buffer.length() > arrowIndex )
                {
                    buffer.remove(buffer.length()  -1 -arrowIndex,1);
                    moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
                    moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
                    moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
                    textCursor().removeSelectedText();
                    textCursor().deletePreviousChar();
                    setTextCursor(storeCursorPos);
                    getUserPrompt();
                    insertPlainText(buffer);
                    storeCursorPos.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, (arrowIndex));
                    setTextCursor(storeCursorPos);
                }
            }
        }
        break;
    }
    case Qt::Key_Left:
    {
        if( buffer.length() > 0 )
            if( buffer.length() > arrowIndex )
            {
                //int bufferLen = buffer.length();
                QTextCursor tmpCursor = textCursor();
                tmpCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
                setTextCursor(tmpCursor);
                arrowIndex++;
            }
        break;
    }
    case Qt::Key_Right:
    {
        if( buffer.length() > 0 )
            if( 0 < arrowIndex )
            {
                QTextCursor tmpCursor = textCursor();
                tmpCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 1);
                setTextCursor(tmpCursor);
                arrowIndex--;
            }
        break;
    }
    // Cycle up through history
    case Qt::Key_Up:
    {
        if(hIndex < history.length() && hIndex >= 0)
        {
            setFocus();
            QTextCursor storeCursorPos = textCursor();
            moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
            textCursor().removeSelectedText();
            textCursor().deletePreviousChar();
            setTextCursor(storeCursorPos);
            buffer = history.at(hIndex);
            getUserPrompt();
            insertPlainText(buffer);
            hIndex--;
        }
        break;
    }
        // Cycle down through history
    case Qt::Key_Down:
    {
        if(history.length() > hIndex+1 )
        {
            setFocus();
            buffer = history.at(hIndex+1);
            QTextCursor storeCursorPos = textCursor();
            moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
            textCursor().removeSelectedText();
            textCursor().deletePreviousChar();
            setTextCursor(storeCursorPos);
            getUserPrompt();
            insertPlainText(buffer);
            hIndex++;
        }
        break;
    }
    case Qt::Key_Return:
    case Qt::Key_Enter:
    {
        if( !buffer.trimmed().isEmpty())
        {
            // Make sure that the cursor gets placed back to the end
            moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);

            // Add input to history, if its full, make room and then add
            if( history.length() <= maxHistory)
                history.append(buffer);//history.push(buffer);
            else
            {
                history.removeLast();
                history.append(buffer);
            }

            // Send data to module for processing
            emit dataReady(buffer);

            buffer = "";
            arrowIndex = 0;
            hIndex = history.length()-1;
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
    // Test buffer cap
    if( buffer.length() <= CAP_INPUT_BUFFER )
    {
        // Find out where in the buffer to put the key
        if( 0 == arrowIndex )
        {
            // Appending to end of text
            buffer += key;
            insertPlainText(key);
        }
        else
        {
            // User moved cursor, place key in buffer at location
            buffer.insert((buffer.length()-arrowIndex), key);

            setFocus();
            QTextCursor storeCursorPos = textCursor();

            moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
            textCursor().removeSelectedText();
            textCursor().deletePreviousChar();
            getUserPrompt();
            insertPlainText(buffer);
            storeCursorPos.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, arrowIndex);
            setTextCursor(storeCursorPos);
        }



    }
    else
    {
        /*
            If cap is reached, delete all of what was entered
            / clear buffer and give back a prompt
        */
        setFocus();
        buffer.remove(buffer.length()-1,buffer.length());
        QTextCursor storeCursorPos = textCursor();
        moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
        moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
        textCursor().removeSelectedText();
        textCursor().deletePreviousChar();
        setTextCursor(storeCursorPos);
        buffer = "";
        getUserPrompt();
        insertPlainText(buffer);
    }
}

void Bkons::getUserPrompt()
{
    appendHtml(("<font color=" + promptColor + ">")
               + currentActivity
               + lqwdText
               + "</font>");
}
