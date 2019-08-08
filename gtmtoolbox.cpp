#include "gtmtoolbox.h"
#include "ui_gtmtoolbox.h"

gtmToolBox::gtmToolBox(QWidget *parent) :
    QToolBox(parent),
    ui(new Ui::gtmToolBox)
{
    ui->setupUi(this);
}

gtmToolBox::~gtmToolBox()
{
    delete ui;
}
