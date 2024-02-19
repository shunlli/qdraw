#ifndef COMMANDWIDGET_H
#define COMMANDWIDGET_H

#include <QPlainTextEdit>
#include <QKeyEvent>
#include <QTextCursor>

#define DEFAULT_PROMPT ">>> "

class CommandWidget : public QPlainTextEdit
{
    Q_OBJECT

public:
    CommandWidget(QWidget *parent = 0);
    void reset();
    QString prompt() const;
    void setPrompt(const QString &prompt);

public slots:
    void result(QString result);
    void append(QString text);

signals:
    void command(QString command);

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    bool inEditionZone(QTextCursor cursor);
    bool inEditionZone();
    void handleEnter();

    QString getCommand() const;

private:
    QString m_prompt{DEFAULT_PROMPT};
    int m_promptBlock{0};
    bool m_locked{false};
};

#endif
