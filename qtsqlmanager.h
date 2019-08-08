#ifndef QTSQLMANAGER_H
#define QTSQLMANAGER_H

#include <QObject>
#include <QtSql/QSqlDatabase>

class QtSqlManager : public QObject
{
    Q_OBJECT
public:
    explicit QtSqlManager(QObject *parent = nullptr,const QString& path="");
protected:
    QSqlDatabase db;

signals:

public slots:
};

#endif // QTSQLMANAGER_H
