#ifndef GTMTOOLBOX_H
#define GTMTOOLBOX_H

#include <QToolBox>

namespace Ui {
class gtmToolBox;
}

class gtmToolBox : public QToolBox
{
    Q_OBJECT

public:
    explicit gtmToolBox(QWidget *parent = nullptr);
    ~gtmToolBox();

private:
    Ui::gtmToolBox *ui;
};

#endif // GTMTOOLBOX_H
