#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QDialog>

namespace Ui {
class MessageBox;
}

class MessageBox : public QDialog
{
    Q_OBJECT

public:
    explicit MessageBox(QString message, QWidget *parent = 0);
    ~MessageBox();

private:
    Ui::MessageBox *ui;
};

#endif // MESSAGEBOX_H
