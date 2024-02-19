#include "commandwidget.h"
#include <QDebug>
#include <QTextBlock>

CommandWidget::CommandWidget(QWidget *parent) : QPlainTextEdit(parent)
{
    setLineWrapMode(NoWrap);
    insertPlainText(m_prompt);
    setUndoRedoEnabled(false);
    setAttribute(Qt::WA_InputMethodEnabled, false);
    setAcceptDrops(false);
    setContextMenuPolicy(Qt::NoContextMenu);
}

void CommandWidget::reset()
{
    clear();
    setPrompt(DEFAULT_PROMPT);
}

bool CommandWidget::inEditionZone(QTextCursor cursor)
{
    int block = cursor.blockNumber();
    int col = cursor.columnNumber();
    if (block > m_promptBlock)
        return true;
    if (block == m_promptBlock && col >= m_prompt.length())
        return true;

    return false;
}

bool CommandWidget::inEditionZone()
{
    QTextCursor cursor = textCursor();
    if (cursor.hasSelection())
    {
        cursor.setPosition(textCursor().selectionStart());
        if (!inEditionZone(cursor))
            return false;
        cursor.setPosition(textCursor().selectionEnd());
        if (!inEditionZone(cursor))
            return false;
    }
    else
    {
        if (!inEditionZone(cursor))
            return false;
    }
    return true;
}

void CommandWidget::keyPressEvent(QKeyEvent *e)
{
    // locked State: a command has been submitted but no result
    // has been received yet.
    if (m_locked)
        return;

    if (textCursor().hasSelection())
    {
        if (e->modifiers() == Qt::CTRL)
        {
            if (e->matches(QKeySequence::Copy))
            {
                QPlainTextEdit::keyPressEvent(e);
                return;
            }
            if (!inEditionZone())
            {
                return;
            }
            QPlainTextEdit::keyPressEvent(e);
            return;
        }
    }

    switch (e->key())
    {
    case Qt::Key_Down:
    case Qt::Key_Up:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_End:
    case Qt::Key_Home:
        QPlainTextEdit::keyPressEvent(e);
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (!inEditionZone())
        {
            return;
        }
        handleEnter();
        break;
    case Qt::Key_Backspace:
    {
        if (!inEditionZone())
        {
            return;
        }
        QTextCursor cursor = textCursor();
        int block = cursor.blockNumber();
        int col = cursor.columnNumber();
        if (block > m_promptBlock)
            QPlainTextEdit::keyPressEvent(e);
        else if (block == m_promptBlock && col > m_prompt.length())
            QPlainTextEdit::keyPressEvent(e);
        return;
    }
    default:
        if (!inEditionZone())
        {
            return;
        }
        QPlainTextEdit::keyPressEvent(e);
        break;
    }
}

// Enter key pressed
void CommandWidget::handleEnter()
{
    QString cmd = getCommand();

    QPlainTextEdit::moveCursor(QTextCursor::EndOfLine);

    if (cmd.length() > 0)
    {
        m_locked = true;
        setFocus();
        insertPlainText("\n");
        emit command(cmd);
    }
    else
    {
        result("");
    }
}

// Result received
void CommandWidget::result(QString result)
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);

    insertPlainText(result);
    insertPlainText("\n");
    insertPlainText(m_prompt);
    ensureCursorVisible();
    m_locked = false;

    m_promptBlock = cursor.blockNumber();
}

// Append line but do not display prompt afterwards
void CommandWidget::append(QString text)
{
    insertPlainText(text);
    insertPlainText("\n");
    ensureCursorVisible();
}

// Select and return the user-input (exclude the prompt)
QString CommandWidget::getCommand() const
{
    auto block = document()->findBlockByNumber(m_promptBlock);
    QTextCursor cursor(block);
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    QString text = cursor.selectedText();
    text.remove(0, m_prompt.length());

    return text;
}

void CommandWidget::setPrompt(const QString &prompt)
{
    m_prompt = prompt;

    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
    insertPlainText("\n");
    insertPlainText(m_prompt);
    m_promptBlock = cursor.blockNumber();
}

QString CommandWidget::prompt() const
{
    return m_prompt;
}
