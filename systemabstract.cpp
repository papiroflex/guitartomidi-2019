#include "systemabstract.h"
SystemAbstract *SystemAbstract::Sys=nullptr;
SystemAbstract::SystemAbstract(QObject *parent) : QObject(parent)
{
    Sys=this;
}
