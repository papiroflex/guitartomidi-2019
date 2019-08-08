#include "qtsqlmanager.h"
#include <qdebug.h>

QtSqlManager::QtSqlManager(QObject *parent,const QString& path) : QObject(parent)
{
    QString dbpath(path);
    if(dbpath=="")
        dbpath=QString("/home/oriol/sdcard/Projects/meu/qtsqlite3.db");
    db.addDatabase("QSQLITE");
    db.setDatabaseName(dbpath);

     if (!db.open())
     {
        qDebug() << "Error: connection with database fail";
     }
     else
     {
        qDebug() << "Database: connection ok";
     }

}
